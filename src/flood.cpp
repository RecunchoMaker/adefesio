/**
 * @file flood.cpp
 *
 * @brief Implementación de flood fill
 *
 */

#include <Arduino.h>
#include <laberinto.h>
#include <robot.h>

/**
 * brief Estructura para almacenar los vecinos de una casilla
 *
 */
typedef struct {
    volatile int8_t tamano;      ///< Numero de vecinos encontrados
    volatile uint8_t casilla[4]; ///< Indice de la casilla de cada vecino
} tipo_vecinos;

volatile tipo_vecinos flood_vecinos; ///< Almacena los vecinos de una casilla

/// Array de distancias de manhatan de cada casilla hasta la casilla solución
volatile uint8_t flood_distancia[MAX_FILAS * MAX_COLUMNAS];


/**
 * brief Inicializa los valores de flood en el laberinto
 *
 * @todo Solo vale cuando la solución es una única casilla
 */
void flood_init(uint8_t origen, uint8_t solucion) {

    uint8_t idx = 0;
    Serial.print(laberinto_get_filas());
    Serial.print(" x ");
    Serial.println(laberinto_get_columnas());
    for (idx = 0; idx < laberinto_get_columnas() * laberinto_get_filas(); idx++) {
        flood_distancia[idx] = abs ( idx / laberinto_get_columnas()) -
                                     solucion / laberinto_get_columnas()
                               +
                               abs ( idx % laberinto_get_columnas() -
                                     solucion % laberinto_get_columnas() );
    }

}

/**
 * brief Devuelve la distancia manhattan de la casilla a la solución
 *
 * @param casilla
 */
uint8_t flood_get_distancia(uint8_t casilla) {
    return flood_distancia[casilla];
}


/**
 * brief Encuentra y almacena en flood_vecinos los vecinos accesibles desde una casilla
 */
void flood_encuentra_vecinos(uint8_t casilla) {

    flood_vecinos.tamano = 0;
    if (!laberinto_hay_pared_norte(casilla))
        flood_vecinos.casilla[flood_vecinos.tamano++] = casilla + incremento[NORTE];
    if (!laberinto_hay_pared_sur(casilla))
        flood_vecinos.casilla[flood_vecinos.tamano++] = casilla + incremento[SUR];
    if (!laberinto_hay_pared_este(casilla))
        flood_vecinos.casilla[flood_vecinos.tamano++] = casilla + incremento[ESTE];
    if (!laberinto_hay_pared_oeste(casilla))
        flood_vecinos.casilla[flood_vecinos.tamano++] = casilla + incremento[OESTE];

}


/**
 * brief Devuelve la casilla mas cercana a la solución con la información actual
 */
uint8_t flood_mejor_vecino_desde(uint8_t casilla) {
    uint8_t minimo = 255;
    uint8_t mejor_casilla;

    flood_encuentra_vecinos(casilla);
    for (uint8_t idx = 0; idx < flood_vecinos.tamano; idx++) {
        if (flood_distancia[flood_vecinos.casilla[idx]] < minimo) {
            minimo = flood_distancia[flood_vecinos.casilla[idx]];
            mejor_casilla = flood_vecinos.casilla[idx];
        }
    }
    return mejor_casilla;
}


/**
 * brief Devuelve el valor del flood del vecino con la mínima distancia a la solución
 */
uint8_t flood_minimo_vecino(uint8_t casilla) {
    uint8_t minimo = 255;

    flood_encuentra_vecinos(casilla);
    for (uint8_t idx = 0; idx < flood_vecinos.tamano; idx++) {
        minimo = min(flood_distancia[flood_vecinos.casilla[idx]], minimo);
    }

    /*
    Serial.print("La minima distancia desde ");
    Serial.print(casilla);
    Serial.print(" es ");
    Serial.println(minimo);
    */

    return minimo;
}


/**
 * brief Recalcula una pasada de valores de flood
 *
 * @returns Valor booleano que indica si hay casillas pendientes
 */
bool flood_recalcula() {

    uint8_t minimo = 0;
    bool hay_pendientes = false;

    for (uint8_t idx = 0; idx < laberinto_get_columnas() * laberinto_get_filas(); idx++) {

        if (flood_distancia[idx] == 0) // es la solucion
            continue;

        minimo = flood_minimo_vecino(idx);
        if (minimo != flood_distancia[idx] - 1) {
            flood_distancia[idx] = minimo + 1;
            hay_pendientes = true;
        }
    }

    return hay_pendientes;
}


