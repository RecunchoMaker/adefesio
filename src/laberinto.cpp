/**
 * @file laberinto.cpp
 *
 * @brief Almacenamiento y tratamiento del laberinto
 *
 * El laberinto se guarda en un array plano de filas x columnas.
 *
 */
#include <Arduino.h>
#include <laberinto.h>
#include <leds.h>
#include <robot.h>
#include <log.h>
#include <flood.h>

//@{
/**
 * @name variables que almacenan el numero de filas y columnas del laberinto actual
 */

volatile uint8_t num_filas = LABERINTO_FILAS;       ///< numero de filas
volatile uint8_t num_columnas = LABERINTO_COLUMNAS; ///< numero de columnas
//@}

/**
 * @brief Estructura que representa una única casilla en el laberinto
 *
 * - paredN: Es 1 si en la celda se detectó una pared al Norte
 * - paredO: Es 1 si en la celda se detecó una pared al Oeste
 * - visitada: Es 1 si la celda ha sido visitada en la exploración
 * - flood: distancia (cartesiana) hacia la solución del laberinto, teniendo en cuenta las paredes 
 *   detectadas hasta el momento
 */
typedef struct {
    volatile char paredN : 1;
    volatile char paredO : 1;
    volatile char visitada: 1;
    volatile uint8_t flood;
} tipo_celda;

/// Array que contiene todas las celdas del laberinto
volatile tipo_celda celda[MAX_FILAS * MAX_COLUMNAS];


void laberinto_init() {

    laberinto_inicializa_valores();
}

void laberinto_inicializa_valores() {
    int idx = 0;
    for (idx = 0; idx < num_columnas * num_filas; idx++) {
        celda[idx].paredN = idx < num_columnas;
        celda[idx].paredO = idx % num_columnas == 0;
        celda[idx].visitada = 0;
    }
}

void laberinto_set_visitada(uint8_t casilla) {
    celda[casilla].visitada = true;
}

void laberinto_get_visitada(uint8_t casilla) {
    return celda[casilla].visitada;
}

bool laberinto_hay_pared_derecha(uint8_t casilla) {
    switch (robot_get_orientacion()) {
        case NORTE: return casilla % num_columnas == num_columnas - 1 or celda[casilla + CASILLA_ESTE].paredO;
                    break;
        case ESTE:  return casilla < num_columnas or celda[casilla + CASILLA_SUR].paredN;
                    break;
        case SUR:   return celda[casilla].paredO;
                    break;
        default: return celda[casilla].paredN;
                    break;
    }
}

bool laberinto_hay_pared_izquierda(uint8_t casilla) {
    switch (robot_get_orientacion()) {
        case NORTE: return celda[casilla].paredO;
                    break;
        case ESTE:  return celda[casilla].paredN;
                    break;
        case SUR:   return casilla % num_columnas == num_columnas -1 or celda[casilla + CASILLA_ESTE].paredO;
                    break;
        default: return casilla < num_columnas or celda[casilla + CASILLA_SUR].paredN;
                    break;
    }
}
    
uint8_t laberinto_get_filas() {
    return num_filas;
}
    
uint8_t laberinto_get_columnas() {
    return num_columnas;
}


void laberinto_set_paredes_laterales(uint8_t casilla, bool izq, bool der) {

    Serial.print("laterales ");
    Serial.print(casilla);
    Serial.print("(");
    Serial.print(robot_get_orientacion());
    Serial.print("): ");
    Serial.print(izq);
    Serial.println(der);
    switch (robot_get_orientacion()) {
        case NORTE: celda[casilla].paredO = izq;
                    if (casilla % num_columnas != num_columnas-1) celda[casilla+CASILLA_ESTE].paredO = der;
                    break;
        case ESTE: celda[casilla].paredN = izq;
                    if (casilla > num_columnas) celda[casilla+CASILLA_SUR].paredN = der;
                    break;
        case SUR:   celda[casilla].paredO = der;
                    if (casilla % num_columnas != num_columnas-1) celda[casilla+CASILLA_ESTE].paredO = izq;
                    break;
        case OESTE: celda[casilla].paredN = der;
                    if (casilla > num_columnas) celda[casilla+CASILLA_SUR].paredN = izq;
                    break;
    }
}

void laberinto_set_pared_frontal(uint8_t casilla, bool frontal) {

    Serial.print("frontal ");
    Serial.print(casilla);
    Serial.print("(");
    Serial.print(robot_get_orientacion());
    Serial.print("): ");
    Serial.println(frontal);

    switch(robot_get_orientacion()) {
        case NORTE: celda[casilla].paredN = frontal;
                    break;
        case ESTE:  if (casilla % num_columnas != num_columnas - 1) celda[casilla + CASILLA_ESTE].paredO = frontal;
                    break;
        case SUR:   if (casilla > num_columnas) celda[casilla + CASILLA_SUR].paredN = frontal;
                    break;
        case OESTE: celda[casilla].paredO = frontal;
                    break;
    }
}


/**
 * @brief Envia al puerto serie una representación gráfica del laberinto
 */
void laberinto_print() {

    int idx = 0;
    uint8_t tipo_linea = 0;

    Serial.print("Orientacion ");
    Serial.println(robot_get_orientacion());




    while (idx < num_filas * num_columnas) {

        switch (tipo_linea) {
        case 0: 
            Serial.print(celda[idx].paredN ? "+-----" : "+     ");
            if (idx % num_columnas == num_columnas - 1)
                Serial.print("+\n");
            break;
        case 1:
            Serial.print(celda[idx].paredO ? "|" : " ");
            Serial.print(celda[idx].visitada ? "." : " ");
            if (flood_get_distancia(idx)< 100) Serial.print(" ");
            if (flood_get_distancia(idx)< 10) Serial.print(" ");
            Serial.print(flood_get_distancia(idx));
            Serial.print(" ");
            if (idx % num_columnas == num_columnas - 1)
                Serial.print("|\n");
            break;
        case 2:
            Serial.print(celda[idx].paredO ? "| " : "  ");
            if (idx == robot_get_casilla()) {
                Serial.print("  ");
                switch(robot_get_orientacion()) {
                    case NORTE: Serial.print("A"); break;
                    case ESTE:  Serial.print(">"); break;
                    case OESTE: Serial.print("<"); break;
                    case SUR:   Serial.print("V"); break;
                }
                Serial.print(" ");
            } else {
                //Serial.print("    ");
                if (idx< 100) Serial.print(" ");
                if (idx< 10) Serial.print(" ");
                Serial.print(idx);
                Serial.print(" ");
            }

            if (idx % num_columnas == num_columnas - 1)
                Serial.print("|\n");
            break;
        }

        idx++;

        if (idx % num_columnas == 0) {
            tipo_linea++;
            if (tipo_linea <= 2) {
                // repito las casillas con otra linea
                idx -= num_columnas;
            } else {
                tipo_linea = 0;
            }
        }
    }
    // Ultima linea
    for (idx = 0; idx < num_columnas; idx++)
        Serial.print("+-----");
    Serial.println("+");
}


bool laberinto_hay_pared_norte(uint8_t casilla) {
    return celda[casilla].paredN;
}

bool laberinto_hay_pared_sur(uint8_t casilla) {
    return casilla > num_columnas or celda[casilla + CASILLA_SUR].paredN;
}

bool laberinto_hay_pared_este(uint8_t casilla) {
    return casilla % num_columnas == num_columnas - 1 or celda[casilla + CASILLA_ESTE].paredO;
}
bool laberinto_hay_pared_oeste(uint8_t casilla) {
    return celda[casilla].paredO;
}
