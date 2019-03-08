#include <Arduino.h>
#include <settings.h>
#include <robot.h>
#include <motores.h>
#include <encoders.h>
#include <leds.h>
#include <timer1.h>
#include <laberinto.h>

#define INFINITO 99999.0
#define RECTO    99999.0
#define ESPERA   99999.0
#define GIRA180  0
#define GIRA90   0

// Aceleracion maxima, usada en las rectas
#define ROBOT_AMAX 1.0

// Aceleracion de frenada, antes de entrar en una curba
#define ROBOT_ACUR 1.0

// Aceleracion de la frenada final en la ultima casilla
#define ROBOT_AFIN 0.8

// Velocidad maxima en recta
#define ROBOT_VR 0.40

// Velocidad maxima en curva
#define ROBOT_VC 0.30

// Velocidad maxima en exploracion
#define ROBOT_VE 0.20

// Velocidad minima antes de parar
#define ROBOT_V0 0.1

// TODO: estas medidas deben estar en otro fichero
#define ROBOT_DIST 0.18
#define ROBOT_DISTG (2*PI*(ROBOT_DIST/2)/4)

tipo_accion accion;

volatile int32_t pasos_recorridos = 0;

volatile float amax = ROBOT_AMAX;
volatile float acur = ROBOT_ACUR;
volatile float afin = ROBOT_AFIN;
volatile float vr = ROBOT_VR;
volatile float vc = ROBOT_VC;
volatile float ve = ROBOT_VE;

volatile bool sinc_pared;

volatile uint8_t aux_cuenta_acciones = 0;

typedef struct {
    volatile tipo_estado estado:3;
    volatile tipo_orientacion orientacion: 2;
    volatile uint8_t casilla = 0;
    volatile float casilla_offset = 0;
} tipo_robot;

volatile tipo_robot robot;

void robot_set_amax(float aceleracion_maxima) {
    amax = aceleracion_maxima;
}

float robot_get_amax() {
    return amax;
}

void robot_set_acur(float aceleracion_entrada_en_curva) {
    acur = aceleracion_entrada_en_curva;
}

float robot_get_acur() {
    return acur;
}

void robot_set_afin(float aceleracion_frenada_final) {
    afin = aceleracion_frenada_final;
}

float robot_get_afin() {
    return afin;
}

void robot_set_vr(float velocidad_en_recta) {
    vr = velocidad_en_recta;
}

float robot_get_vr() {
    return vr;
}

void robot_set_vc(float velocidad_en_curva) {
    vc = velocidad_en_curva;
}

float robot_get_vc() {
    return vc;
}

float _distancia_para_decelerar(float vi, float vf, float aceleracion) {
    return ((0.5 * (vi - vf) * (vi - vf)) + (vf * (vi - vf))) / aceleracion;
}

void _print_accion(tipo_accion accion) {

    Serial.print("* accion: dist:");
    Serial.print(accion.distancia);
    Serial.print(" acel= ");
    Serial.print(accion.aceleracion);
    Serial.print(" vmax="); Serial.print(accion.velocidad_maxima);
    Serial.print(" vfin="); Serial.print(accion.velocidad_final);
    Serial.print(" deceleracion="); Serial.print(accion.deceleracion);
    Serial.print(" pasos_hasta_dec="); Serial.print(accion.pasos_hasta_decelerar);
    Serial.print(" pasos_obj="); Serial.print(accion.pasos_objetivo);
    Serial.print(" r="); Serial.print(accion.radio);

    Serial.println();
}

void _crea_accion(float distancia,
                  float aceleracion, float deceleracion,
                  float velocidad_maxima, float velocidad_final,
                  float radio) {

    // TODO: fix - si no es una recta comleta reseteamos completamente los encoders
    if (distancia == ROBOT_DIST) {
        encoders_decrementa_posicion_total(pasos_recorridos);
    } else {
        encoders_reset_posicion_total();
    }

    // la distancia puede ser negativa en los giros en redondo
    accion.distancia = abs(distancia);
    accion.aceleracion = aceleracion;

    accion.pasos_objetivo = accion.distancia / LONGITUD_PASO_ENCODER;
    accion.deceleracion = deceleracion;

    accion.pasos_hasta_decelerar = ((accion.distancia - _distancia_para_decelerar (velocidad_maxima, velocidad_final, deceleracion)) /
                LONGITUD_PASO_ENCODER);
    accion.velocidad_maxima = velocidad_maxima;
    accion.velocidad_final = velocidad_final;
    accion.radio = radio;


    motores_set_radio(accion.radio);

    if (accion.velocidad_maxima > motores_get_velocidad_lineal_objetivo()) {
        motores_set_aceleracion_lineal(accion.aceleracion);
    }
    else
        motores_set_aceleracion_lineal(0);

    _print_accion(accion);
    timer1_reset_cuenta();
    sinc_pared = false;
}


void robot_init() {
    
    encoders_reset_posicion_total();
    encoders_reset_posicion();
    motores_parar();

    robot.casilla = CASILLA_INICIAL;
    robot.orientacion = ORIENTACION_INICIAL;

    /*
     * Secuencia ADAAII
     */

    /*
    _crea_accion(ROBOT_DIST/2, amax, amax, vr, vr, INFINITO);         // inicial
    _crea_accion(ROBOT_DIST  , amax, acur, vr, vc, INFINITO);         // avanza
    _crea_accion(ROBOT_DISTG , amax, amax, vc, vc, ROBOT_DIST/2);     // derecha
    _crea_accion(ROBOT_DIST  , amax, amax, vr, vr, INFINITO);         // avanza
    _crea_accion(ROBOT_DIST  , amax, acur, vr, vc, INFINITO);         // avanza
    _crea_accion(ROBOT_DISTG , amax, amax, vc, vc, -ROBOT_DIST/2);    // izquierda
    _crea_accion(ROBOT_DISTG , amax, amax, vc, vc, -ROBOT_DIST/2);    // izquierda
    _crea_accion(ROBOT_DIST/2, amax, afin, vc, 0.03, INFINITO);         // final

    // Espera 1 segundo
    _crea_accion(ESPERA, 0.5, 0.5 , 0, 0, 1); // espera 1/2 segundo
    _crea_accion(PI*motores_get_distancia_entre_ruedas()/2, acur, acur , vc, 0.03, GIRA180); // gira 180g
    _crea_accion(ESPERA, 0.5, 0.5 , 0, 0, 1); // espera 1/2 segundo

    // Secuencia IIAADA
    _crea_accion(ROBOT_DIST/2, amax, amax, vc, vc, INFINITO);         // inicial
    _crea_accion(ROBOT_DISTG , amax, amax, vc, vc, ROBOT_DIST/2);     // derecha
    _crea_accion(ROBOT_DISTG , amax, amax, vc, vc, ROBOT_DIST/2);     // derecha
    _crea_accion(ROBOT_DIST  , amax, amax, vr, vr, INFINITO);         // avanza
    _crea_accion(ROBOT_DIST  , amax, acur, vr, vc, INFINITO);         // avanza
    _crea_accion(ROBOT_DISTG , amax, amax, vc, vc, -ROBOT_DIST/2);    // izquierda
    _crea_accion(ROBOT_DIST  , amax, acur, vr, vc, INFINITO);         // avanza
    _crea_accion(ROBOT_DIST/2, amax, afin, vc, 0.03, INFINITO);         // final
    _crea_accion(ESPERA, 0.5, 0.5 , 0, 0, 1); // espera 1/2 segundo
    _crea_accion(PI*motores_get_distancia_entre_ruedas()/2, acur, acur , vc , 0.03, GIRA180); // gira 180g
    */

    // ultima_accion--;
    // accion_idx = 0;
    // accion = accion_array[0];
} 

tipo_accion robot_get_accion() {
    return accion;
}

void _incrementa_casilla() {
    switch (robot.orientacion) {
        case NORTE:
            robot.casilla += CASILLA_NORTE;
            break;
        case SUR:
            robot.casilla += CASILLA_SUR;
            break;
        case ESTE:
            robot.casilla += CASILLA_ESTE;
            break;
        case OESTE:
            robot.casilla += CASILLA_OESTE;
            break;
    }
    laberinto_set_paredes_laterales(robot.casilla, 
            leds_pared_izquierda(),
            leds_pared_derecha()
            );
    sinc_pared = false;
}

void robot_siguiente_accion() {

    if (pasos_recorridos > 0) {
        aux_cuenta_acciones++;
    }

    if (accion.radio == INFINITO and accion.velocidad_final > ROBOT_V0 ) {
        _incrementa_casilla();
        Serial.print("cambio a casilla ");
        Serial.println(robot.casilla);
        laberinto_print();
    }

    // TODO: el offset tampoco es lineal en los giros
    if (accion.radio != 0) robot.casilla_offset += ( pasos_recorridos * LONGITUD_PASO_ENCODER);

    switch (robot.estado) {
        case PARADO:
            motores_parar();
            encoders_reset_posicion_total();
            break;
        case EXPLORANDO_INICIAL:
            _crea_accion(ROBOT_DIST/2, amax, amax, ve, ve, INFINITO);         // inicial
            robot.casilla_offset = ROBOT_DIST / 2;
            robot.estado = EXPLORANDO;
            break;
        case EXPLORANDO:
            if (!leds_pared_derecha()) {
                 Serial.println("paro para giro derecha");
                 _crea_accion(ROBOT_DIST/2, amax, afin, ve, ROBOT_V0, INFINITO);         // final
                 robot.estado = PAUSA_PRE_GIRO;
                 robot.casilla_offset = 0;
            } else if (!leds_pared_enfrente()) {
                 Serial.println("continuo - leds enfrente: ");
                 Serial.print(leds_get_valor(LED_FDER));
                 Serial.print("/");
                 Serial.print(leds_get_valor(LED_FIZQ));
                 _crea_accion(ROBOT_DIST, amax, amax, ve, ve, INFINITO);
            } else if (!leds_pared_izquierda()) {
                 Serial.println("paro para giro izquierda");
                 _crea_accion(ROBOT_DIST/2, amax, afin, ve, ROBOT_V0, INFINITO);         // final
                 robot.estado = PAUSA_PRE_GIRO;
                 robot.casilla_offset = 0;
            } else {
                 Serial.println("callejon sin salida - paro para giro 180");
                 _crea_accion(ROBOT_DIST/2, amax, afin, ve, ROBOT_V0, INFINITO);         // final
                 robot.estado = PAUSA_PRE_GIRO;
                 robot.casilla_offset = 0;
            }
            break;

        case PAUSA_INICIAL:
            Serial.println("pausa inicial");
            _crea_accion(0, 0, 0, 0, 0, 0.2); // espera 
            robot.estado = EXPLORANDO_INICIAL;
            laberinto_print();
            break;

        case PAUSA_PRE_GIRO:
            Serial.println("pausa pregiro");
            _crea_accion(0, 0, 0, 0, 0, 0.2); // espera
            laberinto_set_pared_frontal(robot.casilla, leds_pared_enfrente());
            robot.estado = PARADO_PARA_GIRO;
            break;

        case PARADO_PARA_GIRO:
            if (!leds_pared_derecha()) {
                Serial.println("giro derecha");
                _crea_accion(-PI*motores_get_distancia_entre_ruedas()/4.0, amax, amax , 0.2 , ROBOT_V0, GIRO_DERECHA); // gira 90
                robot.orientacion++;
                robot.casilla_offset = 0;
                robot.estado = PAUSA_INICIAL;
            } else if (!leds_pared_izquierda()) {
                Serial.println("giro izquierda");
                _crea_accion(PI*motores_get_distancia_entre_ruedas()/4.0, amax, amax , 0.2 , ROBOT_V0, GIRO_IZQUIERDA); // gira 90
                robot.orientacion--;
                robot.casilla_offset = 0;
                robot.estado = PAUSA_INICIAL;
            } else {
                Serial.println("giro 180");
                _crea_accion(PI*motores_get_distancia_entre_ruedas()/2.0, amax, amax , 0.2, ROBOT_V0, GIRO_IZQUIERDA); // gira 180g
                robot.orientacion+=2;
                robot.casilla_offset = 0;
                robot.estado = PAUSA_INICIAL;
            }
            break;

        case VUELTA:
            _crea_accion(PI*motores_get_distancia_entre_ruedas()/2, amax, amax , 0.2, ROBOT_V0, GIRA180); // gira 180g
            robot.casilla_offset = 0;
            robot.estado = EXPLORANDO_INICIAL;
            break;

        case TEST:
            if (!leds_pared_enfrente()) {
                _crea_accion(0.01, amax, amax, 0.5, 0.5, INFINITO);
            } else {
                _crea_accion(0.5, afin, 0.5, 0.5, 0.15, INFINITO);
                robot.estado = PARADO;
            }

            break;
    }


    pasos_recorridos = 0;
    if (aux_cuenta_acciones > 250) {
        Serial.println("ultima accion");
        robot.estado = PARADO;
    }
}


uint8_t robot_get_casilla() {
    return robot.casilla;
}

tipo_orientacion robot_get_orientacion() {
    return robot.orientacion;
}

tipo_estado robot_get_estado() {
    return robot.estado;
}

void robot_inicia_exploracion() {

    robot.estado = EXPLORANDO_INICIAL;
    robot.casilla = CASILLA_INICIAL;
    robot.orientacion = ORIENTACION_INICIAL;
    
    laberinto_set_paredes_laterales(robot.casilla, 
            leds_pared_izquierda(), leds_pared_derecha());
    robot_siguiente_accion();
}

float robot_get_casilla_offset() {
    return robot.casilla_offset;
}

void robot_control() {


    // control de posicion
    if (accion.radio!=GIRO_DERECHA and accion.radio!=GIRO_IZQUIERDA and robot.casilla_offset + pasos_recorridos * LONGITUD_PASO_ENCODER > ROBOT_DIST) {
        robot.casilla_offset -= ROBOT_DIST;
    }

    // Sincronizacion con paredes
    if (!leds_pared_derecha() and laberinto_hay_pared_derecha(robot.casilla) and accion.radio == INFINITO and accion.distancia == ROBOT_DIST and !sinc_pared) {
        Serial.println("sinc pared derecha");
        sinc_pared = true;
        // TODO: quedan 4 cm para llegar al final de la casilla
        accion.pasos_objetivo = pasos_recorridos + 0.04 / LONGITUD_PASO_ENCODER;
    }

    if (!leds_pared_izquierda() and laberinto_hay_pared_izquierda(robot.casilla) and accion.radio == INFINITO and accion.distancia == ROBOT_DIST and !sinc_pared) {
        Serial.println("sinc pared derecha");
        sinc_pared = true;
        // TODO: quedan 4 cm para llegar al final de la casilla
        accion.pasos_objetivo = pasos_recorridos + 0.04 / LONGITUD_PASO_ENCODER;
    }

    if ((leds_get_valor(LED_FDER) + leds_get_valor(LED_FIZQ) > 600) and accion.radio == INFINITO) {
        Serial.println("chocamos! leds:");
        Serial.print(leds_get_valor(LED_FDER));
        Serial.print("/");
        Serial.print(leds_get_valor(LED_FIZQ));
        Serial.print(", pasos objetivo =");
        Serial.print(accion.pasos_objetivo);
        Serial.print(" recorridos =");
        Serial.println(pasos_recorridos);
        accion.pasos_objetivo = pasos_recorridos;
    }


    if (accion.velocidad_maxima <= 0.0) {
        motores_parar();
        if (timer1_get_cuenta() * PERIODO_TIMER > accion.radio) {
            robot_siguiente_accion();
        }
    } else {

        pasos_recorridos = accion.radio == GIRO_IZQUIERDA or accion.radio == GIRO_DERECHA ? abs (encoders_get_posicion_total_right() 
            - encoders_get_posicion_total_left()) / 2
            : encoders_get_posicion_total();

        if (pasos_recorridos >= accion.pasos_objetivo or motores_get_velocidad_lineal_objetivo() == 0) {
            robot_siguiente_accion();
        }
        else if (pasos_recorridos >= accion.pasos_hasta_decelerar) {
            motores_set_aceleracion_lineal(-accion.deceleracion);
        }
        else if (motores_get_velocidad_lineal_objetivo() >= accion.velocidad_maxima) {
            motores_set_velocidad_lineal_objetivo(accion.velocidad_maxima);
        }
    }

}
