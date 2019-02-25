#include <Arduino.h>
#include <settings.h>
#include <robot.h>
#include <motores.h>
#include <encoders.h>
#include <leds.h>
#include <timer1.h>

struct tipo_accion {
    volatile int32_t pasos_objetivo;
    volatile int32_t pasos_hasta_decelerar;
    volatile float aceleracion;
    volatile float deceleracion;
    volatile float velocidad_maxima;
    volatile float velocidad_final;
    volatile float radio;
};

#define MAX_ACCIONES 25
#define INFINITO 99999.0
#define RECTO    99999.0
#define ESPERA   99999.0
#define GIRA180  0

// Aceleracion maxima, usada en las rectas
#define ROBOT_AMAX 1.0

// Aceleracion de frenada, antes de entrar en una curba
#define ROBOT_ACUR 1.0

// Aceleracion de la frenada final en la ultima casilla
#define ROBOT_AFIN 1

// Velocidad maxima en recta
#define ROBOT_VR 0.40

// Velocidad maxima en curva
#define ROBOT_VC 0.30

// TODO: estas medidas deben estar en otro fichero
#define ROBOT_DIST 0.18
#define ROBOT_DISTG (2*PI*(ROBOT_DIST/2)/4)

tipo_accion accion;
tipo_accion accion_array[MAX_ACCIONES+1];
volatile uint8_t accion_idx;
volatile uint8_t ultima_accion = 0;

volatile int32_t pasos_recorridos;

volatile float amax = ROBOT_AMAX;
volatile float acur = ROBOT_ACUR;
volatile float afin = ROBOT_AFIN;
volatile float vr = ROBOT_VR;
volatile float vc = ROBOT_VC;

volatile int8_t robot_estado = ROBOT_PARADO;

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
    Serial.print("accion: acel ");
    Serial.print(accion.aceleracion);
    Serial.print(" vmax="); Serial.print(accion.velocidad_maxima);
    Serial.print(" vfin="); Serial.print(accion.velocidad_final);

    Serial.println();
}

void _crea_accion(float distancia,
                  float aceleracion, float deceleracion,
                  float velocidad_maxima, float velocidad_final,
                  float radio) {


    accion_array[ultima_accion].aceleracion = aceleracion;
    accion_array[ultima_accion].pasos_objetivo = distancia / LONGITUD_PASO_ENCODER;
    accion_array[ultima_accion].deceleracion = deceleracion;
    accion_array[ultima_accion].pasos_hasta_decelerar = ((distancia - _distancia_para_decelerar (velocidad_maxima, velocidad_final, deceleracion)) /
                LONGITUD_PASO_ENCODER);
    accion_array[ultima_accion].velocidad_maxima = velocidad_maxima;
    accion_array[ultima_accion].velocidad_final = velocidad_final;
    accion_array[ultima_accion].radio = radio;

    _print_accion(accion_array[ultima_accion]);

    ultima_accion++;
}


void robot_init() {
    
    encoders_reset_posicion_total();
    encoders_reset_posicion();
    motores_parar();

    ultima_accion = 0;

    // _         distancia  , aceleracion, deceleracion, velocidad_maxima, velocidad_final, radio
    // _crea_accion(INFINITO   , 0          , 0.3         , 0.2             , 0.005            , INFINITO); // espera GO

    /* Una recta de dos casillas y un giro de 90 grados
    _crea_accion(0.18*2     , 1          , 1           , 0.3             , 0.1            , RECTO); // avanza
    _crea_accion(ESPERA     , 0.5        , 0.5         , 0               , 0              , 0.5); // espera 1/2 segundo
    _crea_accion(PI*distancia_entre_ruedas/2, 1, 1 , 0.2             , 0.1            , GIRA180); // gira 180g
    */

    /*
     * Pasillo de 6 casillas
     */
    /*
    _crea_accion(ROBOT_DIST/2, amax, amax, vr, vr, INFINITO);         // inicial
    _crea_accion(ROBOT_DIST * 4, amax, amax, vr, vr, INFINITO);         // inicial
    _crea_accion(ROBOT_DIST/2, amax, afin, vc, 0.03, INFINITO);         // final
    _crea_accion(PI*motores_get_distancia_entre_ruedas()/2, acur, acur , vc , 0.03, GIRA180); // gira 180g
    */

    /*
     * Primera accion, dejando las demás dependiendo de los sensores
     */

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

uint8_t robot_get_accion() {
    return accion_idx;
}

void robot_siguiente_accion() {


    switch (robot_estado) {
        case ROBOT_PARADO:
            break;
        case ROBOT_EXPLORANDO:
            ultima_accion = 0;
            Serial.print("sig: ");
            Serial.println(motores_get_velocidad_lineal_objetivo());
            if (motores_get_velocidad_lineal_objetivo() == 0) {
                // Robot parado. Inicio con media casilla hasta vmax
                Serial.print("media casilla acelerando");
                _crea_accion(ROBOT_DIST/2, amax, amax, vc, vc, INFINITO);         // inicial
                //_crea_accion(INFINITO,0, 0.3, 0.2, 0.005,INFINITO); // espera GO
            }
            else if (motores_get_velocidad_lineal_objetivo() < 0.15 and leds_pared_enfrente()) {
                Serial.print("pared enfrente, paro");
                _crea_accion(INFINITO,0, 0.3, 0.2, 0.005,INFINITO); // espera GO
                robot_estado = ROBOT_PARADO;
                motores_parar();
            } else if (motores_get_velocidad_lineal_objetivo() < 0.05 ) {
                // Robot frenando
            } else if (leds_pared_enfrente()) {
                // Robot en movimiento con pared enfrente: freno
                _crea_accion(ROBOT_DIST/2, amax, afin, vc, 0.03, INFINITO);         // final
                Serial.println("pared: freno");
            } else {
                // Robot en movimiento sin pared enfrente. Continuo
                Serial.print("continuo recto");
                _crea_accion(ROBOT_DIST  , amax, amax, vr, vr, INFINITO);         // avanza
            }
            accion = accion_array[0]; // la siguiente se sobreescribira

            motores_set_radio(accion.radio);

            if (accion.velocidad_maxima > motores_get_velocidad_lineal_objetivo()) {
                motores_set_aceleracion_lineal(accion.aceleracion);
            }
            else
                motores_set_aceleracion_lineal(0);

            encoders_reset_posicion_total();
            timer1_reset_cuenta();
            break;
    }
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

int8_t robot_get_estado() {
    return robot_estado;
}

void robot_set_estado(int8_t estado) {
    robot_estado = estado;
}

void robot_control() {

    if (accion.velocidad_maxima == 0.0) {
        motores_parar();
        if (timer1_get_cuenta() * PERIODO_CICLO > accion.radio) {
            robot_siguiente_accion();
        }
    } else {

        pasos_recorridos = accion.radio == 0 ? (encoders_get_posicion_total_right() 
            - encoders_get_posicion_total_left()) / 2
            : encoders_get_posicion_total();

        if (pasos_recorridos >= accion.pasos_objetivo) {
#ifdef ACCIONES_LOG
            Serial.print(timer1_get_cuenta() * PERIODO_TIMER, 5);
            Serial.print(". termina accion en ");
            Serial.print(accion.pasos_objetivo);
            Serial.print(" pasos (real: ");
            Serial.print(encoders_get_posicion_total() * LONGITUD_PASO_ENCODER);
            Serial.print( "m) dif: ");
            Serial.print(encoders_get_posicion_total() - accion.pasos_objetivo);
            Serial.print( "pasos) v:");
            Serial.print(encoders_get_velocidad_left());
            Serial.println(",");
            Serial.print(encoders_get_velocidad_right());
            Serial.println(" m/s");
#endif
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
