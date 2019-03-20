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

/// Flag que controla si la secuencia de encendido de leds está activa
volatile bool leds_activados = false;

/// Contiene para cada led la diferencia de lectura en el sensor entre led encendido y led apagado
volatile int16_t leds_valor[4];

/// Contiene para cada led el valor del sensor cuando el led está apagado. Sólo a efectos de depuración.
volatile int16_t leds_valor_apagado[4];

/// Contiene para cada led el valor del sensor cuando el led está encendido. Sólo a efectos de depuración.
volatile int16_t leds_valor_encendido[4];

/// Valor diferencial entre la actual y la anterior lectura de leds
volatile int16_t leds_valor_d[4];

/// Valor medio de los diodos laterales en la casilla inicial
volatile int16_t leds_valor_medio;


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
    sei();
    leds_calibra();
    cli();
    leds_desactiva();

    leds_apaga(LED_IZQ);
    leds_apaga(LED_DER);
    leds_apaga(LED_FIZQ);
    leds_apaga(LED_FDER);
}


/**
 * @brief Intenta calibrar los valores de los diodos en función de la lectura en la casilla inicial
 */
void leds_calibra() {
    leds_valor_medio = (leds_get_valor(LED_IZQ) + leds_get_valor(LED_DER)) / 2;
    Serial.print("valor medio leds: ");
    Serial.println(leds_valor_medio);
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

    leds_valor_d[led - A0] = leds_lectura0 - leds_lectura1 - leds_valor[led - A0];
    leds_valor[led - A0] = leds_lectura0 - leds_lectura1;
    leds_valor_encendido[led - A0] = leds_lectura1;
    leds_valor_apagado[led - A0] = leds_lectura0;
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
 * @brief Devuelve el valor diferencial entre la actual y la anterior lectura de leds
 *
 * @param led pin digital del led consultado
 *
 */
int16_t leds_get_valor_d(int8_t led) {
    return leds_valor_d[led-A0];
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
    return leds_valor[LED_FDER - A0] > 15 and leds_valor[LED_FIZQ - A0] > 15;
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
 * @brief Devuelve el desvio a la derecha en funcion del valor medio de la calibracion
 */
int16_t leds_get_desvio_derecho() {
    return (leds_valor_medio - leds_valor[LED_DER - A0]);
}


/**
 * @brief Devuelve el desvio a la izquierda en funcion del valor medio de la calibracion
 */
int16_t leds_get_desvio_izquierdo() {
    return (leds_valor[LED_IZQ - A0] - leds_valor_medio);
}


/**
 * @brief Devuelve un valor estimado hasta una pared frontal
 *
 * @todo Evaluar curva característica de respuesta
 */
float leds_distancia_frontal() {
    if (leds_valor[LED_FDER - A0] + leds_valor[LED_FIZQ - A0] > 800) return 0.01;
    if (leds_valor[LED_FDER - A0] + leds_valor[LED_FIZQ - A0] > 700) return 0.015;
    if (leds_valor[LED_FDER - A0] + leds_valor[LED_FIZQ - A0] > 600) return 0.02;
    if (leds_valor[LED_FDER - A0] + leds_valor[LED_FIZQ - A0] > 500) return 0.035;
    if (leds_valor[LED_FDER - A0] + leds_valor[LED_FIZQ - A0] > 400) return 0.05;
    if (leds_valor[LED_FDER - A0] + leds_valor[LED_FIZQ - A0] > 300) return 0.07;
    if (leds_valor[LED_FDER - A0] + leds_valor[LED_FIZQ - A0] > 200) return 0.08;
    return 0.10;
}
