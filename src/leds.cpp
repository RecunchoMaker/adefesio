/**
 * @file leds.cpp
 * @brief Funciones para controlar los leds/sensores infrarrojos
 *
 * Los leds infrarrojos se controlan individualmente, cada uno con un
 * pin digital de salida, y hay un único sensor de entrada en un pin analógico. 
 *
 * Los pins que activan los diodos digitales deben ser adyacentes en el procesador, 
 * para facilitar la aritmética a la hora de almacenar valores en los arrays.
 *
 * @see https://recunchomaker.github.io/adefesio/compartir-pin-de-lectura
 * @see https://recunchomaker.github.io/adefesio/ajustando-posicion-de-sensores/
 *
 * @todo Las funciones get_valor_* no deberían utilizarse en ningún otro fichero
 */

#include <Arduino.h>
#include <leds.h>

#define KALMAN_GAIN 0.1

/// Flag que controla si la secuencia de encendido de leds está activa
volatile bool leds_activados = false;

/// Contiene para cada led la diferencia de lectura en el sensor entre led encendido y led apagado
volatile int16_t leds_valor[4];

/// Contiene para cada led el valor del sensor cuando el led está apagado. Sólo a efectos de depuración.
volatile int16_t leds_valor_apagado[4];

/// Contiene para cada led el valor del sensor cuando el led está encendido. Sólo a efectos de depuración.
volatile int16_t leds_valor_encendido[4];

/// Contiene el valor de la distancia en mm de la última lectura
volatile float leds_distancia[4];

/// Valor diferencial en mm entre la actual y la anterior lectura de leds
volatile float leds_distancia_d[4];

/// Valor acumulado de la distancia para el filtro kalman
volatile float leds_distancia_kalman[4];

// Constantes generadas por leds_obtener_matriz_segmentos.py
#define LEDS_BITS_INDICE_MUESTRA 6
#define LEDS_ESPACIO_MUESTRA 64
/// Constantes para interpolar la distancia a partir de las lecturas analógicas
const uint8_t leds_segmentos[17] = { \
     199 , 97 , 69 , 54 , 44 , 37 , 32 , 27 ,  \
     24 , 21 , 18 , 15 , 13 , 11 , 8 , 2 ,  \
     0 };

/**
 * @brief Inicializa pins y establece el sistema de leds como desactivado
 */

void leds_init() {

    pinMode(LED_IZQ, OUTPUT);
    pinMode(LED_DER, OUTPUT);
    pinMode(LED_FIZQ, OUTPUT);
    pinMode(LED_FDER, OUTPUT);

    pinMode(LED_SENSOR, INPUT);

    leds_activa();
    //@todo es necesarioe esto?
    //sei();
    //leds_calibra();
    //cli();
    leds_desactiva();

    leds_apaga(LED_IZQ);
    leds_apaga(LED_DER);
    leds_apaga(LED_FIZQ);
    leds_apaga(LED_FDER);
}


/**
 * @brief Establece el sistema de leds como activado
 */
void leds_activa() {
    leds_activados = true;
}


/**
 * @brief Establece el sistema de leds como desactivado
 */
void leds_desactiva() {
    leds_activados = false;
    leds_apaga(LED_IZQ);
    leds_apaga(LED_DER);
    leds_apaga(LED_FDER);
    leds_apaga(LED_FIZQ);
}


/**
 * @brief Devuelve el el estado del sistema de leds (activado/desactivado)
 */
bool leds_get_leds_activados() {
    return leds_activados;
}


/**
 * @brief Enciende el led indicado si el sistema está activado
 */
void leds_enciende(int8_t led) {
    if (leds_activados)
        digitalWrite(led, HIGH);
}


/**
 * @brief Apaga el led indicado
 */
void leds_apaga(int8_t led) {
    digitalWrite(led, LOW);
}


/**
 * @brief Actualiza el elemento correspondiente al led indicado en los arrays que guardan distintos datos de los sensores
 *
 * @param led pin digital del led que se quiere actualizar
 *
 * Esta función ejecuta una secuencia de instrucciones:
 * - Lee el valor a la entrada del sensor (se suponen todos los leds apagados, con lo cual tenemos en la entrada un valor correspondiente al "ruido infrarrojo" del ambiente)
 * - Enciende el led indicado y espera el tiempo suficiente para que esté completamente encendido
 * - Lee de nuevo el valor a la entrada del sensor
 * - Apaga el led
 * - Se actualizan los valores de los distintos arrays leds_valor, leds_valor_encendido[]... etc
 *
 * La función está pensada para ser llamada de forma regular (en una interrupción de tiempo)
 *
 */
void leds_actualiza_valor(int8_t led) {

    static int16_t leds_lectura0;
    static int16_t leds_lectura1;

    leds_lectura0 = analogRead(LED_SENSOR);
    leds_enciende(led);
    delayMicroseconds(100);
    leds_lectura1 = analogRead(LED_SENSOR);
    leds_apaga(led);

    leds_valor[led - A0] = leds_lectura0 - leds_lectura1;
    leds_valor_encendido[led - A0] = leds_lectura1;
    leds_valor_apagado[led - A0] = leds_lectura0;

    float distancia_anterior = leds_distancia_kalman[led - A0];
    leds_distancia[led - A0] = leds_interpola_distancia(leds_valor[led-A0]);

    leds_distancia_kalman[led- A0] = KALMAN_GAIN * leds_distancia[led - A0] + (1-KALMAN_GAIN) * distancia_anterior;

    leds_distancia_d[led - A0] = leds_distancia_kalman[led - A0] - distancia_anterior;
}

/**
 * @brief Devuelve la diferencia entre la lectura del sensor con el led encendido/apagado
 *
 * @param led pin digital del led consultado
 *
 */
int16_t leds_get_valor(int8_t led) {
    return leds_valor[led-A0];
}

/**
 * @brief Devuelve la lectura del sensor con el led apagado
 *
 * @param led pin digital del led consultado
 *
 */
int16_t leds_get_valor_apagado(int8_t led) {
    return leds_valor_apagado[led-A0];
}

/**
 * @brief Devuelve la lectura del sensor con el led encendido 
 *
 * @param led pin digital del led consultado
 *
 */
int16_t leds_get_valor_encendido(int8_t led) {
    return leds_valor_encendido[led-A0];
}


/**
 * @brief Devuelve la distancia en mm del led
 *
 * @param led pin digital del led consultado
 *
 */
float leds_get_distancia(int8_t led) {
    return leds_distancia[led-A0];
}


/**
 * @brief Devuelve la distancia en mm del led después de aplicar el filtro Kalman
 *
 * @param led pin digital del led consultado
 *
 */
float leds_get_distancia_kalman(int8_t led) {
    return leds_distancia_kalman[led-A0];
}

/**
 * @brief Devuelve el valor diferencial en mm entre la actual lectura y la anterior
 *
 * @param led pin digital del led consultado
 *
 */
float leds_get_distancia_d(int8_t led) {
    return leds_distancia_d[led-A0];
}


/**
 * @brief Devuelve si existe una pared enfrente
 *
 * Establece que existe una pared si alguna lectura de los leds que apuntan al frente
 * devuelve un valor superior a un umbral.
 *
 * @todo Establecer el valor umbral de otra manera
 */
bool leds_pared_enfrente() {
    return leds_valor[LED_FDER - A0] > 100 and leds_valor[LED_FIZQ - A0] > 100;
}


/**
 * @brief Devuelve si existe una pared a la izquierda 
 *
 * Supone que existe una pared si el led que apunta a la izquierda
 * devuelve un valor superior a un umbral.
 *
 * @todo Establecer el valor umbral de otra manera
 *
 */
bool leds_pared_izquierda() {
    return (leds_valor[LED_IZQ - A0] > 30);
}


/**
 * @brief Devuelve si existe una pared a la derecha 
 *
 * Supone que existe una pared si el led que apunta a la derecha
 * devuelve un valor superior a un umbral.
 *
 * @todo Establecer el valor umbral de otra manera
 */
bool leds_pared_derecha() {
    return (leds_valor[LED_DER - A0] > 20);
}


/**
 * @brief Devuelve un valor estimado en mm a partir de una lectura del ADC
 */
float leds_interpola_distancia(int16_t lectura) {
    
    int8_t indice = lectura >> LEDS_BITS_INDICE_MUESTRA;
    float pendiente = (float) (leds_segmentos[indice+1] - leds_segmentos[indice]) / LEDS_ESPACIO_MUESTRA;
    int16_t espacio = (1 << LEDS_BITS_INDICE_MUESTRA) * indice;

    return 0.001 * (leds_segmentos[indice] + pendiente * (lectura - espacio));

}
