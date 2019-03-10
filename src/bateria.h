/**
 * @file bateria.h
 *
 * @brief Cabecera de bateria.cpp
 */

#ifndef __BATERIA_H
#define __BATERIA_H

#include <Arduino.h>

/// Limite inferior de voltaje aceptado como carga suficiente
#define BATERIA_VOLTAJE_UMBRAL 7.2

/// Limite máximo interpretado como conexión directa por USB
#define BATERIA_VOLTAJE_USB 4.90

/// Pin analógico de entrada para el control del voltaje
#define BATERIA_VOLTAJE_PIN A7

/// Pin digital de salida que controla el led que reporta _bateria descargada_
#define BATERIA_LED_PIN 13

void bateria_init(void);
void bateria_muestra_nivel(void);
void bateria_watchdog(void);
float bateria_get_voltaje();

#endif /* ifndef BATERIA_H

 */
