#ifndef __ACCION_H
#define __ACCION_H

#include <Arduino.h>

#define RADIO_INFINITO 99999

enum tipo_accion {ARRANCAR, PARAR, RECTO, ESPERA,
                  CURVA_DERECHA, CURVA_IZQUIERDA,
                  GIRO_DERECHA, GIRO_IZQUIERDA, GIRO_180};

void accion_set(float distancia,
                float aceleracion, float deceleracion,
                float velocidad_maxima, float velocidad_final,
                float radio);

void accion_ejecuta(char accion);

float accion_get_distancia();
float accion_get_aceleracion();
float accion_get_deceleracion();
float accion_get_velocidad_final();
float accion_get_velocidad_maxima();
float accion_get_radio();
int32_t accion_get_pasos_objetivo();
void accion_set_pasos_objetivo(int32_t pasos);
int32_t accion_get_pasos_hasta_decelerar();
bool accion_cambio_casilla();

void accion_set_amax(float aceleracion_maxima);
float accion_get_amax();
void accion_set_acur(float aceleracion_entrada_en_curva);
float accion_get_acur();
void accion_set_afin(float aceleracion_frenada_final);
float accion_get_afin();
void accion_set_vr(float velocidad_en_recta);
float accion_get_vr();
void accion_set_vc(float velocidad_en_curva);
float accion_get_vc();

#endif /* ifndef ACCION

 */

