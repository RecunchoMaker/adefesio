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
#include <flood.h>
#include <robot.h>
#include <settings.h>
#include <log.h>


volatile uint8_t camino_casilla_origen;
volatile uint8_t camino_orientacion_origen;
volatile uint8_t camino_casilla_actual;
volatile uint8_t camino_orientacion_actual;
volatile uint8_t siguiente;
volatile uint8_t dir;
volatile uint8_t camino_ultima_casilla;
volatile bool camino_todas_visitadas;

/**
 * @brief Inicializa un camino en la celda inicial
 */
void camino_init() {
    camino_init(CASILLA_INICIAL, ORIENTACION_INICIAL);
}


/**
 * @brief Inicializa un camino, en un origen y una orientacion
 *
 * @param origen casilla inicial del camino
 * @param origen orientacion inicial del camino
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
 * @brief Marca un camino de pesos decrecientes de flood
 */
bool camino_recalcula() {

    camino_init(robot_get_casilla(), robot_get_orientacion());
    camino_ultima_casilla = camino_casilla_actual;

    Serial.println(camino_orientacion_actual);


    camino_todas_visitadas = true;
    while (flood_get_distancia(camino_casilla_actual) != 0) {


        siguiente = flood_mejor_vecino_desde(camino_casilla_actual);
        Serial.print(F("mv desde:"));
        Serial.print(camino_casilla_actual);
        Serial.print(" ");
        Serial.println(siguiente);


        // direccion?
        for (dir = 0; dir < 4; dir++) {
            if ((int) siguiente - camino_casilla_actual == incremento[dir]) {

                switch ((camino_orientacion_actual - dir)) {
                    case 0:  camino_anadir_paso(PASO_RECTO); 
                             break;
                    case 1:  
                    case -3:
                             camino_anadir_paso(PASO_IZQ);
                             break;
                    case -1: 
                    case  3: camino_anadir_paso(PASO_DER);
                             break;
                    case 6:
                    case 2:  
                    case -2:  
                    default:
                             camino_anadir_paso(PASO_STOP);
                             Serial.print(F("ERROR!?????"));
                             Serial.println(dir);
                             Serial.println(camino_orientacion_actual - dir);
                             break;
                }
            }
        }

        // bias de camino recto: si la celda no es visitada, continuamos
        // recto (no comprobamos que llegamos al límite. no es necesario
        // ya que en la vida real aparecerá una pared antes
        // -
        if (!laberinto_get_visitada(siguiente)) {
            Serial.print(F("vistadas = false, casill:"));
            Serial.println(siguiente);
            camino_todas_visitadas = false;
            Serial.print(F("anado pasos rectos con orientacion "));
            Serial.println(camino_orientacion_actual);
            for (dir =0; dir < max(laberinto_get_filas(), laberinto_get_columnas()); dir++)
                camino_anadir_paso(PASO_RECTO);
            break;
        } else {
            camino_ultima_casilla = camino_casilla_actual;
        }
    }
    Serial.println(F("Acabo calculo camino"));
    Serial.print(F("todas_visitadas: "));
    Serial.println(camino_todas_visitadas);
    Serial.print(F("ultima casilla"));
    return camino_todas_visitadas;

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
 * @brief Devuelve la ultima casilla del camino calculado 
 */
uint8_t camino_get_ultima_casilla() {
    return camino_ultima_casilla;
}

/**
 * @brief Devuelve true si todas las celdas del camino estan visitadas
 */
bool camino_get_todas_visitadas() {
    return camino_todas_visitadas;
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

    camino_casilla_actual += incremento[camino_orientacion_actual];
    laberinto_set_paso(camino_casilla_actual, PASO_STOP);
}


/**
 * @brief Anade paso recto
 */
void camino_anadir_paso_recto() {
    camino_anadir_paso(PASO_RECTO);
    //laberinto_print();
    log_camino();
}

/**
 * @brief Anade paso izq
 */
void camino_anadir_paso_izq() {
    camino_anadir_paso(PASO_IZQ);
    laberinto_print();
    log_camino();
}

/**
 * @brief Anade paso der 
 */
void camino_anadir_paso_der() {
    camino_anadir_paso(PASO_DER);
    laberinto_print();
    log_camino();
}
