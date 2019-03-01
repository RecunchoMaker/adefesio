#ifndef __LABERINTO_H
#define __LABERINTO_H


#define CASILLA_NORTE (-LABERINTO_COLUMNAS - 1)
#define CASILLA_SUR (LABERINTO_COLUMNAS + 1)
#define CASILLA_ESTE 1
#define CASILLA_OESTE -1


void laberinto_init();
void laberinto_inicializa_bordes();
void laberinto_print();
void laberinto_pon_paredes(uint8_t casilla, bool izq, bool frente, bool der);


#endif


