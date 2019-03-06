#ifndef __LABERINTO_H
#define __LABERINTO_H


#define CASILLA_NORTE (-LABERINTO_COLUMNAS - 1)
#define CASILLA_SUR (LABERINTO_COLUMNAS + 1)
#define CASILLA_ESTE 1
#define CASILLA_OESTE -1


void laberinto_init();
void laberinto_inicializa_bordes();
void laberinto_print();
void laberinto_set_paredes_laterales(uint8_t casilla, bool izq, bool der);
void laberinto_set_pared_frontal(uint8_t casilla);
bool laberinto_hay_pared_derecha(uint8_t casilla);
bool laberinto_hay_pared_izquierda(uint8_t casilla);


#endif


