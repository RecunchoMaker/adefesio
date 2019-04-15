/**
 * @file settings.h
 *
 * @brief Variables globales
 */

#ifndef __SETTINGS_H
#define __SETTINGS_H

/// Constante definida para simulador mms
/// #define MOCK 1

//@{
/**
 * @name Características físicas del robot
 */

/// Radio de las ruedas motrices, en metros
#define RADIO_RUEDAS 0.022992

/// Distancia entre las ruedas, en metros
#define DISTANCIA_ENTRE_RUEDAS (0.068753)

/// Anchura robot
#define ANCHURA_ROBOT 0.085
//@}

//@{
/**
 * @name Configuración del encoder
 */
/// Número de saltos del encoder por cada vuelta completa de rueda
#define PASOS_POR_VUELTA 357.7

/// Metros recorridos por cada paso de encoder
#define LONGITUD_PASO_ENCODER ((2 * PI * RADIO_RUEDAS) / PASOS_POR_VUELTA)
//@}

//@{
/**
 * @name Configuración del timer
 */

#ifdef MOCK
#define PERIODO_TIMER 0.005
#else
#define PERIODO_TIMER 0.001
#endif

/// Distintos estados del timer (cada ciclo de timer se ejecuta un estado distinto)
#define NUMERO_ESTADOS 4

/// Periodo de un ciclo (dependiente del periodo del timer y el numero de estados
#define PERIODO_CICLO (PERIODO_TIMER * NUMERO_ESTADOS)

/// Numero total de OCR1A's contando el numero de estados
#define OCR1A_POR_CICLO (OCR1A * NUMERO_ESTADOS)
//@}

//@{
/**
 * @name Constantes de PID
 *
 * @todo Establecer constantes para los demás pid
 */
#define KP_LINEAL -0.6 ///< Constante P por defecto para pid de velocidad lineal
#define KD_LINEAL -0.0 ///< Constante D por defecto para pid de velocidad lineal
#define KI_LINEAL -0.0 ///< Constante I por defecto para pid de velocidad lineal

//#define KP_PASILLO1 45.0 ///< Constante por defecto P para corrección entre pasillos
//#define KP_PASILLO2 4000.0 ///< Constante por defecto D para corrección entre pasillos
#define KP_PASILLO1 15.0 ///< Constante por defecto P para corrección entre pasillos
#define KP_PASILLO2 1000.0 ///< Constante por defecto D para corrección entre pasillos

/// Define la distancia en la que estimamos incorporarnos al centro en trayectorias rectas
#define DISTANCIA_CONVERGENCIA 0.09
//@}

//@{
/**
 * @name Configuración del laberinto
 */
/*
#define LABERINTO_FILAS 15 ///< Número de filas
#define LABERINTO_COLUMNAS 15 ///< Número de columnas
#define CASILLA_INICIAL 224 ///< Número de casilla en la que se posiciona el robot al inicio de la exploración
#define CASILLA_SOLUCION 14
*/
#define LABERINTO_FILAS 16    ///< Número de filas
#define LABERINTO_COLUMNAS 16 ///< Número de columnas
///< Número de casilla en la que se posiciona el robot al inicio de la exploración. Por defecto ultima fila primera columna
#ifdef MOCK
#define CASILLA_INICIAL (LABERINTO_COLUMNAS * (LABERINTO_FILAS - 1))
#define CASILLA_SOLUCION (LABERINTO_COLUMNAS-1)  
#define ORIENTACION_INICIAL NORTE ///< Orientación del robot al inicio de la exploración
#else
#define CASILLA_INICIAL 240
#define CASILLA_SOLUCION 245
#define ORIENTACION_INICIAL ESTE ///< Orientación del robot al inicio de la exploración
#endif
#define LABERINTO_LONGITUD_CASILLA 0.18 //< Longitud en metros de cada casilla
//@}

#define NOFLOOD 1
#endif /* ifndef SETTINGS_H

*/
