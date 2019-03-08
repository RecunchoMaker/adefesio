#include <Arduino.h>
#include <laberinto.h>
#include <settings.h>
#include <leds.h>
#include <robot.h>
#include <log.h>

#define MAX_FILAS 16
#define MAX_COLUMNAS 16


typedef struct {
    char paredN : 1;
    char paredO : 1;
    char una_variable;
    char otra_variable;
} tipo_celda;

static char num_filas = LABERINTO_FILAS;
static char num_columnas = LABERINTO_COLUMNAS;
static tipo_celda celda[(MAX_FILAS+1) * (MAX_COLUMNAS+1) - 1];

void laberinto_init() {

    laberinto_inicializa_bordes();
}

void laberinto_inicializa_bordes() {
    int idx = 0;
    for (idx = 0; idx < (num_columnas + 1)*(num_filas+1) - 1; idx++) {
        celda[idx].paredN = (idx < num_columnas) or (idx >= num_filas * (num_columnas+1));
        celda[idx].paredO = (idx % CASILLA_SUR == 0 or idx % CASILLA_SUR == num_columnas);
    }
}

bool laberinto_hay_pared_derecha(uint8_t casilla) {
    switch (robot_get_orientacion()) {
        case NORTE: return celda[casilla + CASILLA_ESTE].paredO;
                    break;
        case ESTE:  return celda[casilla + CASILLA_SUR].paredN;
                    break;
        case SUR:   return celda[casilla].paredO;
                    break;
        case OESTE: return celda[casilla].paredN;
                    break;
    }
}

bool laberinto_hay_pared_izquierda(uint8_t casilla) {
    switch (robot_get_orientacion()) {
        case NORTE: return celda[casilla].paredO;
                    break;
        case ESTE:  return celda[casilla].paredN;
                    break;
        case SUR:   return celda[casilla + CASILLA_ESTE].paredO;
                    break;
        case OESTE: return celda[casilla + CASILLA_SUR].paredN;
                    break;
    }
}
    

void laberinto_set_paredes_laterales(uint8_t casilla, bool izq, bool der) {

    log_leds();
    Serial.println();

    switch (robot_get_orientacion()) {
        case NORTE: celda[casilla].paredO = izq;
                    celda[casilla+CASILLA_ESTE].paredO = der;
                    break;
        case ESTE: celda[casilla].paredN = izq;
                    celda[casilla+CASILLA_SUR].paredN = der;
                    break;
        case SUR:   celda[casilla].paredO = der;
                    celda[casilla+CASILLA_ESTE].paredO = izq;
                    break;
        case OESTE: celda[casilla].paredN = der;
                    celda[casilla+CASILLA_SUR].paredN = izq;
                    break;
    }
}

void laberinto_set_pared_frontal(uint8_t casilla, bool frontal) {

    log_leds();
    Serial.println();

    switch(robot_get_orientacion()) {
        case NORTE: celda[casilla].paredN = frontal;
                    break;
        case ESTE:  celda[casilla + CASILLA_ESTE].paredO = frontal;
                    break;
        case SUR:   celda[casilla + CASILLA_SUR].paredN = frontal;
                    break;
        case OESTE: celda[casilla].paredO = frontal;
                    break;
    }
}

void laberinto_print() {

    int idx = 0;
    uint8_t tipo_linea = 0;

    Serial.print("Orientacion ");
    Serial.println(robot_get_orientacion());
    while (idx < (num_filas * (num_columnas+1)) or tipo_linea == 0) {
        switch (tipo_linea) {
        case 0: 
            if (idx % CASILLA_SUR != num_columnas)
                Serial.print(celda[idx].paredN ? "+-----" : "+     ");
            else
                Serial.print("+");
            break;

        case 1:
            Serial.print(celda[idx].paredO ? "| " : "  ");
            if (idx % CASILLA_SUR != num_columnas) {
                if (idx < 100) Serial.print(" ");
                if (idx < 10) Serial.print(" ");
                Serial.print(idx);
                Serial.print(" ");
            }
            break;
        case 2:
            Serial.print(celda[idx].paredO ? "|   " : "    ");
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


