/**
 * @file settings.h
 *
 * @brief Variables globales
 */

#ifndef __SETTINGS_H
#define __SETTINGS_H

//@{
/**
 * @name Características físicas del robot
 */

/// Radio de las ruedas motrices, en metros
#define RADIO_RUEDAS 0.021692

/// Distancia entre las ruedas, en metros
#define DISTANCIA_ENTRE_RUEDAS (0.063753)
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

#define PERIODO_TIMER 0.001

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

#define KP_PASILLO 0.00008 ///< Constante por defecto P para corrección entre pasillos
#define KI_PASILLO 0.1 ///< Constante por defecto D para corrección entre pasillos
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
#define LABERINTO_FILAS 5    ///< Número de filas
#define LABERINTO_COLUMNAS 4 ///< Número de columnas
///< Número de casilla en la que se posiciona el robot al inicio de la exploración. Por defecto ultima fila primera columna
#define CASILLA_INICIAL (LABERINTO_FILAS * LABERINTO_COLUMNAS - LABERINTO_COLUMNAS)
///< Casilla solución. Por defecto primera fila ultima columna.
#define CASILLA_SOLUCION (LABERINTO_COLUMNAS-1)  
#define ORIENTACION_INICIAL NORTE ///< Orientación del robot al inicio de la exploración
#define LABERINTO_LONGITUD_CASILLA 0.18 //< Longitud en metros de cada casilla
//@}


#define FLOOD_FILL

#endif /* ifndef SETTINGS_H

 */
