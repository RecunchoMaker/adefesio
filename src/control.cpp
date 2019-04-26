/**
 * @file control.cpp
 *
 * @brief Funciones específicas de control de velocidad en acciones
 */

#include <motores.h>
#include <timer1.h>
#include <settings.h>
#include <accion.h>
#include <robot.h>
#include <laberinto.h>
#include <leds.h>

#define DISTANCIA_MURO 0.03


void control_continua() {
    robot_siguiente_accion();
}

void control_espera() {

    motores_parar();
    if (timer1_get_cuenta() * PERIODO_TIMER > accion_get_radio()) {
        robot_siguiente_accion();
    }

}


void control_avance() {
    static bool sinc_pared = true;

    if ((leds_get_distancia_d(LED_IZQ) > 0.001 or leds_get_distancia_d(LED_DER) < -0.001 or 
        leds_get_distancia_d(LED_IZQ) > 0.001 or leds_get_distancia_d(LED_DER) < -0.001)
        and !sinc_pared and encoders_get_posicion_total() > 300)
    {
        accion_set_pasos_objetivo(encoders_get_posicion_total() + 0.068 / LONGITUD_PASO_ENCODER);
        sinc_pared = true;
    }

    if (encoders_get_posicion_total() >= accion_get_pasos_objetivo()) {
        sinc_pared = true;
        robot_siguiente_accion();
    }

    if (motores_get_velocidad_lineal_objetivo() >= accion_get_velocidad_maxima()) {
        motores_set_velocidad_lineal_objetivo(accion_get_velocidad_maxima());
        motores_set_aceleracion_lineal(0);
    }

    if (motores_get_velocidad_lineal_objetivo() > 0 and encoders_get_posicion_total() >= accion_get_pasos_hasta_decelerar())
        motores_set_aceleracion_lineal(-accion_get_deceleracion());

}


void control_parada() {

    if (laberinto_hay_pared_frontal(robot_get_casilla())) {
        motores_set_velocidad_lineal_objetivo(min(0.30, 6.0* leds_get_distancia(LED_FDER) - DISTANCIA_MURO));
        if (leds_get_distancia(LED_FDER)<=DISTANCIA_MURO) {
            robot_siguiente_accion();
            Serial.print("Parado en ");
            Serial.print(leds_get_distancia(LED_FIZQ));
            Serial.print(leds_get_distancia("-"));
            Serial.print(leds_get_distancia(LED_FDER));
        }
    } else {
        control_avance(); // si no hay referencia de pared frontal, se realiza un control normal
    }
}

void control_giro() {
    int pasos_recorridos = abs (encoders_get_posicion_total_right() - encoders_get_posicion_total_left()) / 2;
    if (pasos_recorridos >= abs(accion_get_pasos_objetivo())) {
        robot_siguiente_accion();
    }
    if (pasos_recorridos >= abs(accion_get_pasos_hasta_decelerar()))
        motores_set_aceleracion_lineal(-accion_get_deceleracion());
    if (motores_get_velocidad_lineal_objetivo() >= accion_get_velocidad_maxima()) {
        motores_set_velocidad_lineal_objetivo(accion_get_velocidad_maxima());
        motores_set_aceleracion_lineal(0);
    }
}


void control_espera_senal() {
    if (leds_go())
        robot_siguiente_accion();
}
