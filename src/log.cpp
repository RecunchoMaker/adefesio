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
#include <camino.h>

/**
 * @brief Muestra en una línea la diferencia de valores de sensor con led encedido/apagado
 */
void log_leds() {
    Serial.print(F("l:"));
    Serial.print(leds_get_valor(LED_IZQ));
    Serial.print("\t");
    Serial.print(leds_get_valor(LED_FIZQ));
    Serial.print("\t");
    Serial.print(leds_get_valor(LED_FDER));
    Serial.print("\t");
    Serial.print(leds_get_valor(LED_DER));

    Serial.print(F("\tdist:"));
    Serial.print(leds_get_distancia(LED_IZQ));
    Serial.print("\t");
    Serial.print(leds_get_distancia(LED_FIZQ));
    Serial.print("\t");
    Serial.print(leds_get_distancia(LED_FDER));
    Serial.print("\t");
    Serial.print(leds_get_distancia(LED_DER));

    Serial.print(F("\tapagado:"));
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
    Serial.print(F("#5 "));
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
 * @brief Muestra casilla/pasos recorridos/leds y desvios
 *
 * @note Expresión regular para filtrar log: egrep "^#1.[0-9]+.[0-9]+.[0-9]+.[0-9]+.[0-9]+.[0-9]+$" | cut -b 4-
 */
void log_casilla_pasos_leds() {
    Serial.print(F("#1 "));
    Serial.print(robot_get_casilla());
    Serial.print(F("\t"));
    Serial.print(robot_get_pasos_recorridos());
    Serial.print(F("\t"));
    Serial.print(leds_get_distancia(LED_IZQ),4);
    Serial.print("\t");
    Serial.print(leds_get_distancia(LED_FIZQ),4);
    Serial.print("\t");
    Serial.print(leds_get_distancia(LED_FDER),4);
    Serial.print("\t");
    Serial.print(leds_get_distancia(LED_DER),4);
    Serial.print("\t");
    Serial.print(robot_get_angulo_desvio(), 9);
    Serial.println();
}


/**
 * @brief Logea cambio de casilla y leds
 */
void log_cambio_casilla() {
    Serial.print(F("cambio a casilla "));
    Serial.println(robot_get_casilla());
}


/**
 * @brief Imprime lista de casillas de un camino desde el origen
 */
void log_camino() {

    Serial.print("camino: ");
    camino_empieza();
	while (!camino_es_fin()) {
		Serial.print(camino_get_casilla_actual());
		Serial.print(" ");
		camino_siguiente_casilla();
	}
    Serial.println(camino_get_casilla_actual());
}

/**
 * @brief Imprime variables relacionadas con el ajuste de trayectoria en rectas
 */
void log_variables_trayectoria() {
    Serial.print(F("#2 "));
    /*
    Serial.print(robot_get_casilla());
    Serial.print(F("\t"));
    Serial.print(robot_get_pasos_recorridos());
    Serial.print(F("\t"));
    */
    Serial.print(leds_get_distancia_kalman(LED_IZQ), 5);
    Serial.print("\t");
    Serial.print(leds_get_distancia_kalman(LED_DER), 5);
    Serial.print("\t");
    Serial.print(leds_get_distancia_d(LED_IZQ), 5);
    Serial.print("\t");
    Serial.print(leds_get_distancia_d(LED_DER), 5);
    Serial.print("\t");
    Serial.print(robot_get_angulo_desvio(), 9);
    Serial.print("\t");
    Serial.print(motores_get_velocidad_lineal_objetivo_left(), 9);
    Serial.print("\t");
    Serial.print(motores_get_velocidad_lineal_objetivo_right(), 9);
    Serial.print("\t");
    Serial.print(motores_get_radio_aux(), 9);
    Serial.println();
}

/**
 * @brief Muestra distancias minimas de leds
 */
void log_leds_distancias() {
    Serial.print(F("#3 "));
    Serial.print(robot_get_casilla());
    Serial.print(F("\t"));
    Serial.print(robot_get_pasos_recorridos());
    Serial.print(F("\t"));
    Serial.print(leds_get_distancia(LED_IZQ),9);
    Serial.print(F("\t"));
    Serial.print(leds_get_distancia(LED_FIZQ),9);
    Serial.print(F("\t"));
    Serial.print(leds_get_distancia(LED_FDER),9);
    Serial.print(F("\t"));
    Serial.print(leds_get_distancia(LED_DER),9);
    Serial.print(F("\t"));
    Serial.print(leds_get_pasos_distancia_minima(LED_IZQ),9);
    Serial.print(F("\t"));
    Serial.print(leds_get_pasos_distancia_minima(LED_FIZQ),9);
    Serial.print(F("\t"));
    Serial.print(leds_get_pasos_distancia_minima(LED_FDER),9);
    Serial.print(F("\t"));
    Serial.print(leds_get_pasos_distancia_minima(LED_DER),9);

    Serial.println();
}

/**
 * @brief Muestra las variables implicadas en la correcion en pasillos
 */
void log_correccion_pasillos() {
    Serial.print(F("#4 "));

    Serial.print(robot_get_casilla());
    Serial.print(F("\t"));
    Serial.print(robot_get_pasos_recorridos());
    Serial.print(F("\t"));
    LOGF(encoders_get_velocidad_left(),5);
    LOGF(encoders_get_velocidad_right(),5);
    Serial.print(robot_es_valido_led_izquierdo()? 3:2);
    Serial.print(F("\t"));
    Serial.print(robot_es_valido_led_derecho()?1:2);
    Serial.print(F("\t"));
    Serial.print(motores_get_kp_pasillo1() * (-leds_get_distancia_kalman(LED_IZQ) - (ANCHURA_ROBOT / 2.0) + (LABERINTO_LONGITUD_CASILLA/2.0)),5);
    Serial.print(F("\t"));
    Serial.print(-motores_get_kp_pasillo2() * (leds_get_distancia_d(LED_IZQ) * motores_get_velocidad_lineal_objetivo()),5);
    Serial.print(F("\t"));
    Serial.print(-motores_get_kp_pasillo1() * (-leds_get_distancia_kalman(LED_DER) - (ANCHURA_ROBOT / 2.0) + (LABERINTO_LONGITUD_CASILLA/2.0)),5);
    Serial.print(F("\t"));
    Serial.print(motores_get_kp_pasillo2() * (leds_get_distancia_d(LED_DER) * motores_get_velocidad_lineal_objetivo()),5);
    Serial.print(F("\t"));
    Serial.print(robot_get_angulo_desvio(),5);
    Serial.println();
}

