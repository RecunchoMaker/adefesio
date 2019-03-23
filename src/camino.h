/**
 * @file camino.h
 *
 * @brief Cabecera de camino.cpp
 */

#ifndef __CAMINO_H
#define __CAMINO_H

#include <Arduino.h>

enum tipo_paso {PASO_RECTO, PASO_DER, PASO_IZQ, PASO_STOP};

void camino_init(uint8_t origen, uint8_t orientacion);
void camino_empieza();
uint8_t camino_get_casilla_origen();
uint8_t camino_get_casilla_actual();
uint8_t camino_get_orientacion_origen();
bool camino_es_fin();
void camino_siguiente_casilla();
void camino_anadir_paso(tipo_paso paso);

#endif /* ifndef _CAMINO_H

 */
