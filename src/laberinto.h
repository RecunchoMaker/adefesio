/**
 * @file laberinto.h
 *
 * @brief Cabecera de laberinto.cpp
 *
 */
#ifndef __LABERINTO_H
#define __LABERINTO_H

#include <Arduino.h>
#include <settings.h>
#include <camino.h>

#define MAX_FILAS 16
#define MAX_COLUMNAS 16

/// Array que contiene los distintos incrementos asociados a direcciones N,E,S y O
const int8_t incremento[4] = {-LABERINTO_COLUMNAS,
                               1,
                               LABERINTO_COLUMNAS,
                               -1};


void laberinto_init();
void laberinto_inicializa_valores();
void laberinto_print();
void laberinto_set_paredes_laterales(uint8_t casilla, bool izq, bool der);
void laberinto_set_pared_frontal(uint8_t casilla, bool frontal);
bool laberinto_hay_pared_derecha(uint8_t casilla);
bool laberinto_hay_pared_izquierda(uint8_t casilla);

bool laberinto_hay_pared_norte(uint8_t casilla);
bool laberinto_hay_pared_sur(uint8_t casilla);
bool laberinto_hay_pared_este(uint8_t casilla);
bool laberinto_hay_pared_oeste(uint8_t casilla);

void laberinto_set_visitada(uint8_t casilla);
char laberinto_get_visitada(uint8_t casilla);
tipo_paso laberinto_get_paso(uint8_t casilla);

void laberinto_set_paso(uint8_t casilla, tipo_paso paso);

uint8_t laberinto_get_filas();
uint8_t laberinto_get_columnas();

#endif


