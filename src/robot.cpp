#include <Arduino.h>
#include <settings.h>
#include <robot.h>
#include <motores.h>
#include <encoders.h>
#include <timer1.h>


struct tipo_accion {
    volatile int32_t pasos_objetivo;
    volatile int32_t pasos_hasta_decelerar;
    volatile float aceleracion;
    volatile float deceleracion;
    volatile float velocidad_maxima;
    volatile float radio;
};

#define MAX_ACCIONES 10
#define INFINITO 99999.0
#define RECTO    99999.0
#define ESPERA   99999.0
#define GIRA180  0


tipo_accion accion;
tipo_accion accion_array[MAX_ACCIONES+1];
volatile uint8_t accion_idx;
volatile uint8_t ultima_accion = 0;

volatile int32_t pasos_recorridos;


float _distancia_para_decelerar(float velocidad, float aceleracion) {
    return 0.5 * velocidad * velocidad / aceleracion;
}

void _crea_accion(float distancia,
                  float aceleracion, float deceleracion,
                  float velocidad_maxima, float velocidad_final,
                  float radio) {
    accion_array[ultima_accion].aceleracion = aceleracion;
    accion_array[ultima_accion].pasos_objetivo = distancia / LONGITUD_PASO_ENCODER;
    accion_array[ultima_accion].deceleracion = deceleracion;
    accion_array[ultima_accion].pasos_hasta_decelerar = (int32_t) ((distancia - _distancia_para_decelerar (velocidad_maxima - velocidad_final, deceleracion)) /
                LONGITUD_PASO_ENCODER);
    Serial.println("aqui");
    Serial.println(ultima_accion);
    Serial.println(_distancia_para_decelerar(0.3,0.5));
    Serial.println(distancia - _distancia_para_decelerar (velocidad_maxima - velocidad_final, deceleracion)); 
    Serial.println(accion_array[ultima_accion].pasos_hasta_decelerar);
    Serial.println("-");

    accion_array[ultima_accion].velocidad_maxima = velocidad_maxima;
    accion_array[ultima_accion].radio = radio;
    ultima_accion++;
}

void _print_accion(tipo_accion accion) {
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
}


void robot_init() {
    
    encoders_reset_posicion_total();
    encoders_reset_posicion();
    motores_parar();

    // _         distancia  , aceleracion, deceleracion, velocidad_maxima, velocidad_final, radio
    _crea_accion(INFINITO   , 0          , 0.3         , 0.2             , 0              , INFINITO); // espera GO

    /* Una recta de dos casillas y un giro de 90 grados
    _crea_accion(0.18*2     , 1          , 1           , 0.3             , 0.1            , RECTO); // avanza
    _crea_accion(ESPERA     , 0.5        , 0.5         , 0               , 0              , 0.5); // espera 1/2 segundo
    _crea_accion(PI*DISTANCIA_ENTRE_RUEDAS/2, 1, 1 , 0.2             , 0.1            , GIRA180); // gira 180g
    */


    // Secuencia ADAAII
#define AMAX 1.0
#define ACUR 0.6
#define AFIN 0.6
#define VR 0.2
#define VC 0.15
#define DIST 0.18
#define DISTG (2*PI*(DIST/2)/4)

    _crea_accion(DIST/2, AMAX, AMAX, VR, VR, INFINITO);   // inicial
    _crea_accion(DIST  , AMAX, ACUR, VR, VC, INFINITO);   // avanza
    _crea_accion(DISTG , AMAX, AMAX   , VC, VC, DIST/2);  // derecha
    _crea_accion(DIST  , AMAX, AMAX   , VR, VR, INFINITO);// avanza
    _crea_accion(DIST  , AMAX, ACUR, VR, VC, INFINITO);   // avanza
    _crea_accion(DISTG , AMAX, AMAX   , VC, VC, -DIST/2); // izquierda
    _crea_accion(DISTG , AMAX, AMAX   , VC, VC, -DIST/2); // izquierda
    _crea_accion(DIST/2, AMAX, AFIN, VC, VC, INFINITO);   // final

    ultima_accion--;
    accion_idx = 0;
    accion = accion_array[0];
} 

uint8_t robot_get_accion() {
    return accion_idx;
}


void robot_siguiente_accion() {

    Serial.print("Encoders: ");
    Serial.print(encoders_get_posicion_total_left());
    Serial.print(" (");
    Serial.print(encoders_get_posicion_total_right() * LONGITUD_PASO_ENCODER);
    Serial.print("m), ");
    Serial.print(encoders_get_posicion_total_right());
    Serial.print(" (");
    Serial.print(encoders_get_posicion_total_right() * LONGITUD_PASO_ENCODER);
    Serial.println("m) ");

    if (accion_idx < ultima_accion ) {
        accion_idx++;
    } else {
        accion_idx = 0;
        motores_parar();
    }
    accion = accion_array[accion_idx];
    motores_set_radio(accion.radio);
    motores_set_aceleracion_lineal(accion.aceleracion);

    Serial.print("---- siguiente accion: ");
    Serial.println(accion_idx);
    _print_accion(accion);

    accion = accion_array[accion_idx];
    encoders_reset_posicion_total();
    timer1_reset_cuenta();
}

void robot_control() {

    if (accion.velocidad_maxima == 0.0) {
        motores_parar();
        if (timer1_get_cuenta() * PERIODO_TIMER > accion.radio) {
            robot_siguiente_accion();
        }
    } else {

        pasos_recorridos = accion.radio == 0 ? (encoders_get_posicion_total_right() 
            - encoders_get_posicion_total_left()) / 2
            : encoders_get_posicion_total();
        /*
        Serial.print(accion.radio);
        Serial.print(" ");
        Serial.print(accion.pasos_objetivo);
        Serial.print(" ");
        Serial.print(pasos_recorridos);
        Serial.print(" ");
        Serial.print(encoders_get_posicion_total_right());
        Serial.print(" ");
        Serial.println(encoders_get_posicion_total());
        */

        if (pasos_recorridos >= accion.pasos_objetivo) {
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

            robot_siguiente_accion();
        }
        else if (pasos_recorridos >= accion.pasos_hasta_decelerar) {
            Serial.print(timer1_get_cuenta() * PERIODO_TIMER, 5);
            Serial.print(". decelero en ");
            Serial.print(pasos_recorridos);
            Serial.print(" pasos, quedan: ");
            Serial.print((accion.pasos_objetivo - pasos_recorridos) * LONGITUD_PASO_ENCODER);
            Serial.println(" m");
            motores_set_aceleracion_lineal(-accion.deceleracion);

            accion.pasos_hasta_decelerar = INFINITO ; // no volver a entrar en este if
            timer1_reset_cuenta();
        }
        else if (motores_get_velocidad_lineal_objetivo() >= accion.velocidad_maxima) {
                /*
                Serial.print(timer1_get_cuenta() * PERIODO_TIMER, 5);
                Serial.print(". vmax: ");
                Serial.print(motores_get_velocidad_lineal_objetivo(),4);
                Serial.print(" alcanzada");
                Serial.println();
                */

                motores_set_aceleracion_lineal(0);
        }
    }
    motores_actualiza_velocidad();

}
