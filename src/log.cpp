/**
 * @file log.cpp
 *
 * @brief Funciones generales para logear distintas variables
 */
#include <Arduino.h>
#include <log.h>
#include <leds.h>
#include <accion.h>
#include <robot.h>
#include <motores.h>
#include <encoders.h>
#include <laberinto.h>
#include <bateria.h>

/**
 * @brief Muestra en una línea la diferencia de valores de sensor con led encedido/apagado
 */
void log_leds() {
    Serial.print(F("leds:"));
    Serial.print(leds_get_valor(LED_IZQ));
    Serial.print("\t");
    Serial.print(leds_get_valor(LED_FIZQ));
    Serial.print("\t");
    Serial.print(leds_get_valor(LED_FDER));
    Serial.print("\t");
    Serial.print(leds_get_valor(LED_DER));

    Serial.print("\tencendido:");
    Serial.print(leds_get_valor_encendido(LED_IZQ));
    Serial.print("\t");
    Serial.print(leds_get_valor_encendido(LED_FIZQ));
    Serial.print("\t");
    Serial.print(leds_get_valor_encendido(LED_FDER));
    Serial.print("\t");
    Serial.print(leds_get_valor_encendido(LED_DER));

    Serial.print("\tapagado:");
    Serial.print(leds_get_valor_apagado(LED_IZQ));
    Serial.print("\t");
    Serial.print(leds_get_valor_apagado(LED_FIZQ));
    Serial.print("\t");
    Serial.print(leds_get_valor_apagado(LED_FDER));
    Serial.print("\t");
    Serial.println(leds_get_valor_apagado(LED_DER));
}

/**
 * @brief Muestra datos de la _accion_ actual
 */
void log_accion() {
    Serial.print(F("* accion: dist:"));
    Serial.print(accion_get_distancia());
    Serial.print(F(" acel= "));
    Serial.print(accion_get_aceleracion());
    Serial.print(F(" vmax=")); Serial.print(accion_get_velocidad_maxima());
    Serial.print(F(" vfin=")); Serial.print(accion_get_velocidad_final());
    Serial.print(F(" deceleracion=")); Serial.print(accion_get_deceleracion());
    Serial.print(F(" pasos_hasta_dec=")); Serial.print(accion_get_pasos_hasta_decelerar());
    Serial.print(F(" pasos_obj=")); Serial.print(accion_get_pasos_objetivo());
    Serial.print(F(" r=")); Serial.print(accion_get_radio());

    Serial.println();
}

/**
 * @brief Muestra datos de encoders, velocidades y pwm
 */
void log_motores() {
    LOGF(encoders_get_velocidad_left(),5);
    LOGF(encoders_get_velocidad_right(),5);
    LOG(motores_get_pwm_left());
    LOG(motores_get_pwm_right());
    LOG(encoders_get_posicion_total_left());
    LOG(encoders_get_posicion_total_right());
    LOGF(motores_get_velocidad_lineal_objetivo(),5);
    LOGN(encoders_get_ticks_sin_actualizar_right());
}

/**
 * @brief Muestra datos de pasos objetivo y recorridos
 */
void log_pasos() {
    Serial.print(F("pasos "));
    LOG(robot_get_pasos_recorridos());
    Serial.print(F("/"));
    LOGN(accion_get_pasos_objetivo());
}

/**
 * @brief Muestra casilla/pasos recorridos/leds
 *
 * @note Expresión regular para filtrar log: egrep "^#1.[0-9]+.[0-9]+.[0-9]+.[0-9]+.[0-9]+.[0-9]+$" | cut -b 4-
 */
void log_casilla_pasos_leds() {
    Serial.print(F("#1 "));
    Serial.print(robot_get_casilla());
    Serial.print(F("\t"));
    Serial.print(robot_get_pasos_recorridos());
    Serial.print(F("\t"));
    Serial.print(leds_get_valor(LED_IZQ));
    Serial.print("\t");
    Serial.print(leds_get_valor(LED_FIZQ));
    Serial.print("\t");
    Serial.print(leds_get_valor(LED_FDER));
    Serial.print("\t");
    Serial.print(leds_get_valor(LED_DER));
    Serial.print("\t");
    Serial.println(bateria_get_voltaje());
}


/**
 * @brief Logea cambio de casilla y leds
 */
void log_cambio_casilla() {
    Serial.print(F("cambio a casilla "));
    Serial.println(robot_get_casilla());
    log_leds();
}
