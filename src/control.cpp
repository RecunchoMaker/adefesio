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
#include <leds.h>

#define DISTANCIA_MURO 0.025


void control_continua() {
    robot_siguiente_accion();
}

void control_espera() {

    motores_parar();
    if (timer1_get_cuenta() * PERIODO_TIMER > accion_get_radio()) {
        robot_siguiente_accion();
    }

}

void control_parada() {

    if (leds_pared_enfrente()) {
        motores_set_velocidad_lineal_objetivo(min(0.30, 6.0* leds_get_distancia(LED_FDER) - DISTANCIA_MURO));
        if (leds_get_distancia(LED_FDER)<=DISTANCIA_MURO) {
            robot_siguiente_accion();
            Serial.print("Parado en ");
            Serial.print(leds_get_distancia(LED_FIZQ));
            Serial.print(leds_get_distancia("-"));
            Serial.print(leds_get_distancia(LED_FDER));
        }

    }
}

