/**
 * @file flood.h
 *
 * @brief Cabecera de flood.cpp
 */
#ifndef __FLOOD_H
#define __FLOOD_H

#include <settings.h>
#include <laberinto.h>

void flood_init(uint8_t origen, uint8_t solucion);
uint8_t flood_get_distancia(uint8_t casilla);
uint8_t flood_minimo_vecino(uint8_t casilla);
uint8_t flood_mejor_vecino_desde(uint8_t casilla);
void flood_encuentra_vecinos(uint8_t casilla);
bool flood_recalcula();

#endif /* ifndef FLOOD

 */
