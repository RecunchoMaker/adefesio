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

typedef struct {
    volatile tipo_estado estado:2;
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
    return (0.5 * (vi - vf) * (vi - vf)) + (vf * (vi - vf) / aceleracion);
}

void _print_accion(tipo_accion accion) {

    //laberinto_print();

    Serial.print("accion: acel ");
    Serial.print(accion.aceleracion);
    Serial.print(" vmax="); Serial.print(accion.velocidad_maxima);
    Serial.print(" vfin="); Serial.print(accion.velocidad_final);
    Serial.print(" deceleracion="); Serial.print(accion.deceleracion);
    Serial.print(" pasos_hasta_dec="); Serial.print(accion.pasos_hasta_decelerar);
    Serial.print(" pasos_obj="); Serial.print(accion.pasos_objetivo);

    Serial.println();
}

void _crea_accion(float distancia,
                  float aceleracion, float deceleracion,
                  float velocidad_maxima, float velocidad_final,
                  float radio) {


    accion.aceleracion = aceleracion;
    accion.pasos_objetivo = distancia / LONGITUD_PASO_ENCODER;
    accion.deceleracion = deceleracion;
    accion.pasos_hasta_decelerar = ((distancia - _distancia_para_decelerar (velocidad_maxima, velocidad_final, deceleracion)) /
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

    encoders_decrementa_posicion_total(pasos_recorridos);

    //_print_accion(accion);

}


void robot_init() {
    
    encoders_reset_posicion_total();
    encoders_reset_posicion();
    motores_parar();

    robot.casilla = CASILLA_INICIAL;
    robot.orientacion = ORIENTACION_INICIAL;
    robot.casilla_offset = ROBOT_DIST / 2; // empezamos a mitad de casilla;


    laberinto_pon_paredes(robot.casilla, 
            leds_pared_izquierda(),
            leds_pared_enfrente(),
            leds_pared_derecha()
            );

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
    Serial.println("incremento casilla!");
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
    Serial.println(robot.casilla);
}

void robot_siguiente_accion() {

    robot.casilla_offset += ( pasos_recorridos * LONGITUD_PASO_ENCODER);

    // control de posicion
    if (robot.casilla_offset > ROBOT_DIST) {
        _incrementa_casilla();
        robot.casilla_offset -= ROBOT_DIST;
        laberinto_pon_paredes(robot.casilla, 
                leds_pared_izquierda(),
                leds_pared_enfrente(),
                leds_pared_derecha()
                );
    }

    switch (robot.estado) {
        case PARADO:
            motores_parar();
            encoders_reset_posicion_total();
            break;
        case EXPLORANDO:

            if (!leds_pared_enfrente()) {
                _crea_accion(0.02, amax, amax, ve, ve, INFINITO);
            } else {
                Serial.print("pared detectada en casilla ");
                Serial.print(robot.casilla);
                Serial.print(":");
                Serial.println(leds_distancia_frontal());
                laberinto_pon_paredes(robot.casilla, 
                        leds_pared_izquierda(),
                        leds_pared_enfrente(),
                        leds_pared_derecha()
                        );
                _crea_accion(ROBOT_DIST/2.0 - leds_distancia_frontal(), afin, afin, ve, 0.00, INFINITO);
                robot.estado = PARADO;
            }


            /*
            Serial.print("explorando. v = ");
            Serial.println(motores_get_velocidad_lineal_objetivo());
            if (motores_get_velocidad_lineal_objetivo() == 0) {
                // Robot parado. Inicio con media casilla hasta vmax
                Serial.println("media casilla acelerando");
                _crea_accion(ROBOT_DIST/2, amax, amax, vc, vc, INFINITO);         // inicial
                //_crea_accion(INFINITO,0, 0.3, 0.2, 0.005,INFINITO); // espera GO
            }
            else if (motores_get_velocidad_lineal_objetivo() < 0.15 and leds_pared_enfrente()) {
                Serial.println("pared enfrente, paro");
                _crea_accion(INFINITO,0, 0.3, 0.2, 0.005,INFINITO); // espera GO
                robot.estado = ROBOT_PARADO;
                motores_parar();
            } else if (motores_get_velocidad_lineal_objetivo() < 0.05 ) {
                // Robot frenando
            } else if (leds_pared_enfrente()) {
                // Robot en movimiento con pared enfrente: freno
                _crea_accion(ROBOT_DIST/2, amax, afin, vc, 0.03, INFINITO);         // final
                _incrementa_casilla();
                Serial.println("pared: freno");
            } else {
                // Robot en movimiento sin pared enfrente. Continuo
                Serial.println("continuo recto");
                _incrementa_casilla();
                _crea_accion(ROBOT_DIST  , amax, amax, vr, vr, INFINITO);         // avanza
            }
            */

            timer1_reset_cuenta();
            break;
    }
    pasos_recorridos = 0;
}

/*
void robot_siguiente_accion() {

    motores_set_velocidad_lineal_objetivo(accion.velocidad_final);

    if (accion_idx < ultima_accion ) {

        accion_idx++;
    } else {
        accion_idx = 0;
        motores_parar();
    }
    accion = accion_array[accion_idx];

    motores_set_radio(accion.radio);

    if (accion.velocidad_maxima > motores_get_velocidad_lineal_objetivo()) {
        motores_set_aceleracion_lineal(accion.aceleracion);
    }
    else
        motores_set_aceleracion_lineal(0);


#ifdef ACCIONES_LOG
    Serial.print("Encoders: ");
    Serial.print(encoders_get_posicion_total_left());
    Serial.print(" (");
    Serial.print(encoders_get_posicion_total_right() * LONGITUD_PASO_ENCODER);
    Serial.print("m), ");
    Serial.print(encoders_get_posicion_total_right());
    Serial.print(" (");
    Serial.print(encoders_get_posicion_total_right() * LONGITUD_PASO_ENCODER);
    Serial.println("m) ");

    Serial.print("---- siguiente accion: ");
    Serial.println(accion_idx);

    Serial.print(accion.pasos_objetivo > INFINITO ? -1 : accion.pasos_objetivo * LONGITUD_PASO_ENCODER);
    Serial.print("m (");
    Serial.print(accion.pasos_objetivo > INFINITO ? -1 : accion.pasos_objetivo);
    Serial.print(" pasos) r=");
    Serial.print(accion.radio == INFINITO ? -1 : accion.radio);
    Serial.print("m - acc/dec:");
    Serial.print(accion.aceleracion);
    Serial.print("/");
    Serial.print(accion.deceleracion);
    Serial.print("m/ss ; decelerar en ");
    Serial.print(accion.pasos_hasta_decelerar > INFINITO ? -1 : accion.pasos_hasta_decelerar);
    Serial.print(" (");
    Serial.print(accion.pasos_hasta_decelerar > INFINITO ? -1 : accion.pasos_hasta_decelerar * LONGITUD_PASO_ENCODER);
    Serial.print("m.) ; v max: ");
    Serial.print(accion.velocidad_maxima);
    Serial.print("m/s");
    Serial.println();
#endif

    accion = accion_array[accion_idx];
    encoders_reset_posicion_total();
    timer1_reset_cuenta();
}
*/

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

    robot.estado = EXPLORANDO;
    robot.casilla = CASILLA_INICIAL;
    robot.orientacion = ORIENTACION_INICIAL;
    
    robot_siguiente_accion();
}

float robot_get_casilla_offset() {
    return robot.casilla_offset;
}

void robot_control() {

    if (accion.velocidad_maxima <= 0.0) {
        motores_parar();
        if (timer1_get_cuenta() * PERIODO_CICLO > accion.radio) {
            robot_siguiente_accion();
        }
    } else {

        pasos_recorridos = accion.radio == 0 ? (encoders_get_posicion_total_right() 
            - encoders_get_posicion_total_left()) / 2
            : encoders_get_posicion_total();

        if (pasos_recorridos >= accion.pasos_objetivo) {
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
