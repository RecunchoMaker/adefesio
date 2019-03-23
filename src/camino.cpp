/**
 * @file camino.cpp
 *
 * @brief Gestiona caminos almacenados en laberinto
 *
 * Un camino es una secuencia de movimientos guardados en el array de casillas
 * del laberinto. No hay información de velocidades y la correspondencia con
 * acciones se establece en función de varios parámetros.
 *
 * Cada casilla puede contener un paso, que indica si el camino trazado
 * debe segir recto o torcer hacia algún lado. Los pasos individuales no
 * tienen sentido si no se consideran como un todo desde la casilla origen.
 */
#include <Arduino.h>
#include <camino.h>
#include <laberinto.h>


uint8_t camino_casilla_origen;
uint8_t camino_orientacion_origen;
uint8_t camino_casilla_actual;
uint8_t camino_orientacion_actual;

/**
 * @brief Inicializa un camino, en un origen y una orientacion
 */
void camino_init(uint8_t origen, uint8_t orientacion) {
    camino_casilla_origen = origen;
    camino_orientacion_origen = orientacion;
    camino_casilla_actual = origen;
    camino_orientacion_actual = orientacion;

    laberinto_set_paso(camino_casilla_actual, PASO_STOP);
}


/**
 * @brief Inicia las variables temporales para poder seguir un camino
 */
void camino_empieza() {
    camino_casilla_actual = camino_casilla_origen;
    camino_orientacion_actual = camino_orientacion_origen;
}


/**
 * @brief Avanza un paso actualizando casilla y orientacion actual
 */
void camino_siguiente_casilla() {
    if (laberinto_get_paso(camino_casilla_actual) == PASO_DER)
        camino_orientacion_actual++;
    else if (laberinto_get_paso(camino_casilla_actual) == PASO_IZQ)
        camino_orientacion_actual--;
    camino_orientacion_actual = camino_orientacion_actual % 4;

    camino_casilla_actual += incremento[camino_orientacion_actual];
}


/**
 * @brief getter de casilla_origen
 */
uint8_t camino_get_casilla_origen() {
    return camino_casilla_origen;
}


/**
 * @brief getter de casilla_origen
 */
uint8_t camino_get_casilla_actual() {
    return camino_casilla_actual;
}


/**
 * @brief Devuelve si estamos en el paso final de un camino
 */
bool camino_es_fin() {
    return laberinto_get_paso(camino_casilla_actual) == PASO_STOP;
}


/**
 * @brief getter de orientacion_origen
 */
uint8_t camino_get_orientacion_origen() {
    return camino_orientacion_origen;
}


/**
 * @brief Anade un paso al camino
 *
 * @param tipo_paso paso, puede ser unicamente RECTO, IZQ, DER o PARAR
 */
void camino_anadir_paso(tipo_paso paso) {
    laberinto_set_paso(camino_casilla_actual, paso);
    if (paso == PASO_IZQ) camino_orientacion_actual--;
    else if (paso == PASO_DER) camino_orientacion_actual++;
    camino_orientacion_actual = camino_orientacion_actual % 4;

    Serial.print("paso = ");
    Serial.print(paso);
    Serial.print(" y paso de ");
    Serial.print(camino_casilla_actual);
    camino_casilla_actual += incremento[camino_orientacion_actual];
    Serial.print(" a ");
    Serial.println(camino_casilla_actual);
    laberinto_set_paso(camino_casilla_actual, PASO_STOP);
}
