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



#define CASILLA_NORTE -LABERINTO_COLUMNAS
#define CASILLA_SUR LABERINTO_COLUMNAS
#define CASILLA_ESTE 1
#define CASILLA_OESTE -1

#define MAX_FILAS 16
#define MAX_COLUMNAS 16

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
void laberinto_get_visitada(uint8_t casilla);

uint8_t laberinto_get_filas();
uint8_t laberinto_get_columnas();

#endif


