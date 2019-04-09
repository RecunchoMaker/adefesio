/**
 * @file leds.h
 *
 * @brief Cabecera de leds.cpp
 */

#ifndef __LEDS_H
#define __LEDS_H

/// Pin digital de salida que enciende/apaga el led izquierdo que apunta al frente
#define LED_FIZQ A0

/// Pin digital de salida que enciende/apaga el led que apunta a la izquierda
#define LED_IZQ A2

/// Pin digital de salida que enciende/apaga el led que apunta a la derecha
#define LED_DER A1

/// Pin digital de salida que enciende/apaga el led derecho que apunta al frente
#define LED_FDER A3

/// Pin analógico de entrada compartido por todos los sensores infrarrojos
#define LED_SENSOR A4

void leds_init();
void leds_calibra();
void leds_recalibra();
void leds_activa();
void leds_desactiva();
bool leds_get_leds_activados();
void leds_enciende(int8_t led);
void leds_apaga(int8_t led);
void leds_actualiza_valor(int8_t sensor);
int16_t leds_get_valor(int8_t sensor);
int16_t leds_get_valor_apagado(int8_t sensor);
int16_t leds_get_valor_encendido(int8_t sensor);
float leds_get_distancia_d(int8_t sensor);
float leds_get_distancia(int8_t sensor);
float leds_get_distancia_kalman(int8_t sensor);
float leds_get_distancia_minima(int8_t led);
void leds_reset_distancias_minimas();
int16_t leds_get_pasos_distancia_minima(int8_t led);
int16_t leds_get_diferencia_pasos_der_izq();
int16_t leds_get_pasos_minima_lectura_frontal();

float leds_interpola_distancia(int16_t lectura);
bool leds_pared_enfrente();
bool leds_pared_derecha();
bool leds_pared_izquierda();

bool leds_go();
void leds_reset_go();

#endif
