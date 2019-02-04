#include <Arduino.h>
#include <settings.h>
#include <robot.h>
#include <motores.h>
#include <encoders.h>
#include <timer1.h>


struct tipo_accion {
    volatile uint32_t pasos_objetivo;
    volatile uint32_t pasos_hasta_decelerar;
    volatile float aceleracion;
    volatile float deceleracion;
    volatile float velocidad_maxima;
    volatile float radio;
};

#define ULTIMA_ACCION 10
#define ACELERACION_RECTA 1.5 
#define INFINITO 99999.0

tipo_accion accion;
tipo_accion accion_array[ULTIMA_ACCION+1];
volatile uint8_t accion_idx;
volatile uint8_t ultima_accion = 0;

float _distancia_para_decelerar(float velocidad, float aceleracion) {
    return velocidad * velocidad / (aceleracion * 2);
}

void _crea_accion(float distancia,
                  float aceleracion, float deceleracion,
                  float velocidad_maxima, float velocidad_final,
                  float radio) {
    accion_array[ultima_accion].aceleracion = aceleracion;
    accion_array[ultima_accion].pasos_objetivo = distancia / LONGITUD_PASO_ENCODER;
    accion_array[ultima_accion].deceleracion = deceleracion;
    accion_array[ultima_accion].pasos_hasta_decelerar = (distancia - _distancia_para_decelerar (velocidad_maxima - velocidad_final, deceleracion)) /
                LONGITUD_PASO_ENCODER;
    accion_array[ultima_accion].velocidad_maxima = velocidad_maxima;
    if (radio == 0)
        Serial.println("ERROR!");
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
    _crea_accion(INFINITO   , 0          , 0.3         , 0.2             , 0              , INFINITO);
    _crea_accion(0.18+0.09  , 0.5        , 0.3         , 0.2             , 0              , INFINITO);
    _crea_accion((PI*0.09/2), 0.3        , 0.3         , 0.2             , 0.2            , 0.09);
    _crea_accion(0.18+0.09  , 0.5        , 0.3         , 0.2             , 0              , INFINITO);
    _crea_accion((PI*0.09/2), 0.3        , 0.3         , 0.2             , 0.2            , 0.09);
    _crea_accion(0.18+0.09  , 0.5        , 0.3         , 0.2             , 0              , INFINITO);
    _crea_accion((PI*0.09/2), 0.3        , 0.3         , 0.2             , 0.2            , 0.09);
    _crea_accion(0.18+0.09  , 0.5        , 0.3         , 0.2             , 0              , INFINITO);

    ultima_accion--;
    accion_idx = 0;
    accion = accion_array[0];

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
}

void robot_control() {
    if (encoders_get_posicion_total() >= accion.pasos_objetivo) {
        Serial.print("Alcanzo pasos ");
        Serial.print(accion.pasos_objetivo);
        Serial.print(" a ");
        Serial.print(motores_get_velocidad_lineal_objetivo());
        Serial.println(" m/s");
        robot_siguiente_accion();
    }
    else if (encoders_get_posicion_total() > accion.pasos_hasta_decelerar) {
        Serial.print("decelero en ");
        Serial.print((accion.pasos_objetivo - accion.pasos_hasta_decelerar)  * LONGITUD_PASO_ENCODER);
        Serial.print("=");
        Serial.print(encoders_get_posicion_total() * LONGITUD_PASO_ENCODER);
        Serial.print("m - vel=");
        Serial.println(encoders_get_velocidad_right());
        motores_set_aceleracion_lineal(-accion.deceleracion);
        accion.pasos_hasta_decelerar = INFINITO ; // no volver a entrar en este if
        timer1_reset_cuenta();

    }
    else
        if (motores_get_velocidad_lineal_objetivo() >= accion.velocidad_maxima) {
            motores_set_aceleracion_lineal(0);
        }
    motores_actualiza_velocidad();

}
