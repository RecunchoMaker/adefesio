#include <Arduino.h>
#include <laberinto.h>
#include <settings.h>
#include <robot.h>

#define MAX_FILAS 16
#define MAX_COLUMNAS 16


typedef struct {
    char paredN : 1;
    char paredE : 1;
    char una_variable;
    char otra_variable;
} tipo_celda;

char num_filas = LABERINTO_FILAS;
char num_columnas = LABERINTO_COLUMNAS;
tipo_celda celda[(MAX_FILAS+1) * (MAX_COLUMNAS+1) - 1];

void laberinto_init() {

    laberinto_inicializa_bordes();
}

void laberinto_inicializa_bordes() {
    int idx = 0;
    for (idx = 0; idx < (num_columnas + 1)*(num_filas+1) - 1; idx++) {
        celda[idx].paredN = (idx < num_columnas) or (idx >= num_filas * (num_columnas+1));
        celda[idx].paredE = (idx % CASILLA_SUR == 0 or idx % CASILLA_SUR == num_columnas);
    }
}

void laberinto_pon_paredes(uint8_t casilla, bool izq, bool frente, bool der) {
    celda[casilla].paredN = frente;
    celda[casilla].paredE = izq;
    celda[casilla+CASILLA_ESTE].paredE = der;
    laberinto_print();
}

void laberinto_print() {

    int idx = 0;
    uint8_t tipo_linea = 0;

    while (idx < (num_filas * (num_columnas+1)) or tipo_linea == 0) {
        switch (tipo_linea) {
        case 0: 
            if (idx % CASILLA_SUR != num_columnas)
                Serial.print(celda[idx].paredN ? "+-----" : "+     ");
            else
                Serial.print("+");
            break;

        case 1:
            Serial.print(celda[idx].paredE ? "| " : "  ");
            if (idx % CASILLA_SUR != num_columnas) {
                if (idx < 100) Serial.print(" ");
                if (idx < 10) Serial.print(" ");
                Serial.print(idx);
                Serial.print(" ");
            }
            break;
        case 2:
            Serial.print(celda[idx].paredE ? "|   " : "    ");
            if (idx == robot_get_casilla()) {
                switch(robot_get_orientacion()) {
                    case NORTE: Serial.print("A"); break;
                    case ESTE:  Serial.print(">"); break;
                    case OESTE: Serial.print("<"); break;
                    case SUR:   Serial.print("V"); break;
                }
            } else Serial.print(" ");
            Serial.print(" ");
            break;
        }

        idx++;

        if (idx % CASILLA_SUR == 0) {
            Serial.println();
            tipo_linea++;
            if (tipo_linea <= 2) {
                idx -= CASILLA_SUR;
            } else {
                tipo_linea = 0;
            }
        }
    }

    Serial.println();


}


