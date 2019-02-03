#include <Arduino.h>
#include <settings.h>
#include <robot.h>
#include <motores.h>
#include <encoders.h>


struct tipo_accion {
    uint32_t pasos_objetivo;
    uint32_t pasos_hasta_decelerar;
    float deceleracion;
    float velocidad_maxima;
    float radio;
};

#define ULTIMA_ACCION 1
struct tipo_accion accion;
struct tipo_accion accion_array[ULTIMA_ACCION+1];
uint8_t accion_idx;

float _distancia_para_decelerar(float velocidad, float aceleracion) {
    return velocidad * velocidad / (aceleracion * 2);
}

void robot_init() {

    float distancia;
    uint32_t pasos;
    float deceleracion;
    float velocidad_maxima;
    float velocidad_final;
    float radio;
    
    accion_array[0].pasos_objetivo = 9999;
    accion_array[0].pasos_hasta_decelerar = 9999;
    accion_array[0].deceleracion = 0;
    accion_array[0].velocidad_maxima = 0;
    accion_array[0].radio = 9999;

    encoders_reset_posicion_total();
    encoders_reset_posicion();
    motores_set_potencia(0,0);
    motores_set_velocidad(0, 0);

    distancia = 0.5;
    pasos = distancia / LONGITUD_PASO_ENCODER;
    deceleracion = 0.5;
    velocidad_maxima = 0.2;
    velocidad_final = 0;
    radio = 9999;

    accion_array[1].pasos_objetivo = pasos;
    accion_array[1].pasos_hasta_decelerar = (distancia - _distancia_para_decelerar(velocidad_maxima - velocidad_final, deceleracion)) / LONGITUD_PASO_ENCODER;
    accion_array[1].deceleracion = deceleracion;
    accion_array[1].velocidad_maxima = velocidad_maxima;
    accion_array[1].radio = radio;

    accion_idx = 0;
    accion = accion_array[0];

} 

void robot_siguiente_accion() {
    accion = accion_array[accion_idx];
    if (accion_idx < ULTIMA_ACCION) {
        accion = accion_array[++accion_idx];

        if (motores_get_velocidad_lineal_objetivo_temp() < accion.velocidad_maxima) {
            if (accion.radio = 9999)
                motores_set_aceleracion_lineal(0.1);
            motores_set_radio(accion.radio);
        }
    } else {
        accion_idx = 0;
        accion = accion_array[0];
        motores_parar();
    }
    encoders_reset_posicion_total();
    Serial.print("accion: ");
    Serial.println(accion_idx);
}

void robot_control() {
    if (encoders_get_posicion_total() >= accion.pasos_objetivo) {
        Serial.print("Alcanzo pasos ");
        Serial.print(accion.pasos_objetivo);
        Serial.print(" a ");
        Serial.print(motores_get_velocidad_lineal_objetivo_temp());
        Serial.println(" m/s");
        robot_siguiente_accion();
    }
    else if (encoders_get_posicion_total() > accion.pasos_hasta_decelerar) {
        accion.pasos_hasta_decelerar = 999999; // no volver a entrar en este if
        Serial.print("decelero en ");
        Serial.println(encoders_get_posicion_total());
        motores_set_aceleracion_lineal(-accion.deceleracion);
    }
    else
        if (motores_get_velocidad_lineal_objetivo_temp() >= accion.velocidad_maxima) {
            motores_set_aceleracion_lineal(0);
        }
    motores_actualiza_velocidad();
}
