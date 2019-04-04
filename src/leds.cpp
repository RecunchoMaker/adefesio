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
#include <bateria.h>
#include <robot.h>

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

/// Valor minimo de distancia
volatile float leds_distancia_minima[4];

/// Pasos en los que se ha guardado la distancia minima;
volatile int16_t leds_pasos_distancia_minima[4];

/// Sentidos creciente (1) o decreciente (0) de la distancia en cada leg. 1 bit por led
volatile uint8_t leds_sentido = 0;

// Constantes generadas por leds_obtener_matriz_segmentos.py
#define LEDS_BITS_INDICE_MUESTRA 6
#define LEDS_ESPACIO_MUESTRA 64
/// Constantes para interpolar la distancia a partir de las lecturas analógicas
const uint8_t leds_segmentos[17] = { 
     199 , 97 , 69 , 54 , 44 , 37 , 32 , 27 ,
     24 , 21 , 18 , 15 , 13 , 11 , 8 , 2 ,
     0 };

/// Constantes de correccion para cada led
const float leds_correccion[4] = {   // FIZQ,DER,IZQ,FDER
    0.007, 0.000, -0.01, 0.000
};

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
 * @brief Pone al maximo el array de distancias minimas 
 */
void leds_reset_distancias_minimas() {
    leds_distancia_minima[0] = 0.2;
    leds_distancia_minima[1] = 0.2;
    leds_distancia_minima[2] = 0.2;
    leds_distancia_minima[3] = 0.2;
    leds_pasos_distancia_minima[0] = 0;
    leds_pasos_distancia_minima[1] = 0;
    leds_pasos_distancia_minima[2] = 0;
    leds_pasos_distancia_minima[3] = 0;
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

    float distancia_anterior = leds_distancia[led - A0];
    leds_distancia[led - A0] = leds_interpola_distancia(leds_valor[led-A0]) + leds_correccion[led-A0];

    leds_distancia_kalman[led- A0] = KALMAN_GAIN * leds_distancia[led - A0] + (1-KALMAN_GAIN) * distancia_anterior;

    leds_distancia_d[led - A0] = leds_distancia_kalman[led - A0] - distancia_anterior;

    if (leds_distancia[led - A0] < leds_distancia_minima[led-A0]) {
        leds_distancia_minima[led-A0] = leds_distancia[led-A0];
        leds_pasos_distancia_minima[led-A0] = robot_get_pasos_recorridos();
    }
    /*
        if (leds_distancia_d[led - A0] < 0)
            leds_distancia_minima[led - A0] = leds_distancia_kalman[led - A0];
    }
    else 
        leds_minima_distancia[led - A0] = 0;
    // actualiza sentidos y máximas distancias
    if (leds_distancia_kalman[led - A0] < 0.07)
        if (leds_distancia_d[led - A0] < 0)
           if (leds_sentido & 1 << (led-A0)) {
                Serial.print(" - maximo en ");
                Serial.print(led - A0);
                Serial.print(" -  ");
                Serial.print(leds_distancia_kalman[led - A0], 9);
                leds_sentido &= 0 << (led - A0);
                Serial.print(" -  ");
                Serial.println(leds_sentido, BIN);
                leds_
            } else {
                leds_sentido &= 0 << (led - A0);
            }

        else {
            //Serial.println(leds_sentido, BIN);
            leds_sentido |= 1 << (led - A0);
        }
        */

        /*
        if (leds_distancia_d[led - A0] > 0) {
            leds_sentido |= 1 << (led - A0);
            Serial.print(leds_distancia_d[led - A0], 8);

        }
        else {
            leds_sentido &= 0 << (led - A0);
            Serial.print("            ");
            Serial.print(leds_distancia_d[led - A0], 8);
        }
        Serial.print(" ");
        Serial.println(leds_sentido, BIN);
        */
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
 * @brief Devuelve la minima distancia detectada desde el ultimo reset
 *
 * @param led pin digital del led consultado
 *
 */
float leds_get_distancia_minima(int8_t led) {
    return leds_distancia_minima[led-A0];
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
 * @brief Devuelve la media de pasos de los sensores frontales a los que se ha obtenido la lectura minima
 *
 * @param led pin digital del led consultado
 *
 */
int16_t leds_get_pasos_minima_lectura_frontal() {
    return (leds_pasos_distancia_minima[LED_FIZQ-A0] + leds_pasos_distancia_minima[LED_FDER-A0]) / 2;
}


/**
 * @brief Devuelve los pasos a los que se ha muestreado la distancia minima
 *
 * @param led pin digital del led consultado
 *
 */
int16_t leds_get_pasos_distancia_minima(int8_t led) {
    return leds_pasos_distancia_minima[led-A0];
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
    return leds_distancia[LED_FDER - A0] < 0.16 and leds_distancia[LED_FIZQ - A0] < 0.16;
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
    return (leds_distancia[LED_IZQ - A0] < 0.10);
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
    return (leds_distancia[LED_DER - A0] < 0.10);
}


/*
 * @brief Devuelve la diferencia de pasos entre las lecturas mínimas de los led IZQ y DER
 */
int16_t leds_get_diferencia_pasos_der_izq() {
    return leds_pasos_distancia_minima[LED_IZQ-A0] - leds_pasos_distancia_minima[LED_DER - A0];
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

/**
 * @brief Devuelve true cuando se detecta que se ha puesto y retirado algo delante del sensor frontal izquierdo
 */
bool leds_go() {

    static uint8_t leds_frontal_go_estado = 0;

    if (robot_get_estado() == PARADO and leds_get_distancia_kalman(LED_FIZQ) < 0.05)
        leds_frontal_go_estado = 1;
    if (robot_get_estado() == PARADO and leds_get_distancia_kalman(LED_FIZQ) > 0.10)  {
        leds_frontal_go_estado = (leds_frontal_go_estado == 1 ? 2 : 0);
    }
    digitalWrite(BATERIA_LED_PIN, leds_frontal_go_estado == 1);
    return (leds_frontal_go_estado == 2);
}
