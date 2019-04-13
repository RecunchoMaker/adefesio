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
void flood_init(int16_t solucion) {

    int16_t idx = 0;

    for (idx = 0; idx < laberinto_get_columnas() * laberinto_get_filas(); idx++) {
        flood_distancia[idx] = abs ( idx / laberinto_get_columnas() -
                                     solucion / laberinto_get_columnas())
                               +
                               abs ( idx % laberinto_get_columnas() -
                                     solucion % laberinto_get_columnas() );
#ifdef MOCK
        Serial.print("flood ");
        Serial.print(idx);
        Serial.print(" ");
        Serial.println(flood_distancia[idx]);
#endif
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
 * brief Devuelve true si vecino es accesible desde casilla
 */
bool flood_es_vecino(uint8_t origen, uint8_t vecino) {

    flood_encuentra_vecinos(origen);
    for (uint8_t i = 0; i < flood_vecinos.tamano; i++)
        if (flood_vecinos.casilla[i] == vecino)
            return true;
    return false;
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

    /*
    Serial.print("vecinos de ");
    Serial.println(casilla);
    */

    flood_encuentra_vecinos(casilla);
    for (uint8_t idx = 0; idx < flood_vecinos.tamano; idx++) {
        // Serial.println(flood_vecinos.casilla[idx]);
        
        minimo = min(flood_distancia[flood_vecinos.casilla[idx]], minimo);
    }

    return minimo;
}


/**
 * brief Recalcula una pasada de valores de flood
 *
 * @returns Valor booleano que indica si hay casillas pendientes
 */
bool flood_recalcula() {
#ifdef NOFLOOD
    return false;
#endif

    uint8_t minimo = 0;
    bool hay_pendientes = false;

    for (int16_t idx = 0; idx < laberinto_get_columnas() * laberinto_get_filas(); idx++) {

        if (flood_distancia[idx] == 0) // es la solucion
            continue;

        minimo = flood_minimo_vecino(idx);
        if (minimo != flood_distancia[idx] - 1) {
            flood_distancia[idx] = minimo + 1;
            hay_pendientes = true;
#ifdef MOCK
            Serial.print("flood2 ");
            Serial.print(idx);
            Serial.print(" ");
            Serial.println(minimo+1);
#endif

        }
    }

    return hay_pendientes;
}


