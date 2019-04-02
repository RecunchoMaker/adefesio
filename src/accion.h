/**
 * @file accion.h
 *
 * @brief Cabecera de accion.cpp
 */
#ifndef __ACCION_H
#define __ACCION_H

#include <Arduino.h>

/// Constante interpretada como radio infinito (para las rectas, equivalemte a 99.999 Km)
#define RADIO_INFINITO 99999

/// Constante precalculada equivalente a los pasos de encoder necesarios para recorrer 2/3 de una casilla
#define DOS_TERCIOS_CASILLA (int)((2*LABERINTO_LONGITUD_CASILLA)/(3*LONGITUD_PASO_ENCODER))

/// Aceleracion maxima por defecto, usada en las rectas
#define ACCION_AMAX 1.0

/// Aceleracion de frenada antes de entrar en una curva
#define ACCION_ACUR 1.0

/// Aceleracion de la frenada final en la ultima casilla
#define ACCION_AFIN 0.8

/// Velocidad máxima en recta
#define ACCION_VR 0.40

/// Velocidad máxima en curva
#define ACCION_VC 0.30

/// Velocidad máxima en exploracion
#define ACCION_VE 0.30

/// Velocidad máxima giro en redonddo 
#define ACCION_VG 0.20

/// Velocidad minima antes de parar
#define ACCION_V0 0.1

/// Longitud del arco que se describe en una curva
#define ACCION_DISTG (2*PI*(LABERINTO_LONGITUD_CASILLA/2)/4)


/**
 * @brief Tipos de acciones
 *
 * @todo Actualmente sólo estan consideradas las acciones necesarias para la exploración
 */
enum tipo_accion {ARRANCA, PARA, AVANZA, ESPERA,
                  GIRA_DER, GIRA_IZQ, GIRA_180};

float accion_get_distancia();
float accion_get_aceleracion();
float accion_get_deceleracion();
float accion_get_velocidad_final();
float accion_get_velocidad_maxima();
float accion_get_radio();
tipo_accion accion_get_accion_actual();
int32_t accion_get_pasos_objetivo();
void accion_set_pasos_objetivo(int32_t pasos);
int32_t accion_get_pasos_hasta_decelerar();
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
void accion_set_ve(float velocidad_en_exploracion);
float accion_get_ve();

void accion_set(float distancia,
                float aceleracion, float deceleracion,
                float velocidad_maxima, float velocidad_final,
                float radio);
void accion_ejecuta(tipo_accion accion);
void accion_interrumpe(int32_t pasos);
bool accion_cambio_casilla();


#endif /* ifndef ACCION

 */

