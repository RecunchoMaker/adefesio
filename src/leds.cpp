#include <Arduino.h>
#include <leds.h>

volatile bool leds_activados = false;
volatile int16_t leds_valor[4];
volatile int16_t leds_valor_apagado[4];
volatile int16_t leds_valor_encendido[4];
volatile int16_t leds_valor_d[4];

volatile int16_t leds_lectura0;
volatile int16_t leds_lectura1;

volatile uint32_t led_aux;

void leds_init() {

    pinMode(LED_IZQ, OUTPUT);
    pinMode(LED_DER, OUTPUT);
    pinMode(LED_FIZQ, OUTPUT);
    pinMode(LED_FDER, OUTPUT);

    pinMode(LED_SENSOR, INPUT);

    leds_apaga(LED_IZQ);
    leds_apaga(LED_DER);
    leds_apaga(LED_FIZQ);
    leds_apaga(LED_FDER);

    leds_activados = false;
}

void leds_activa() {
    leds_activados = true;
}

void leds_desactiva() {
    leds_activados = false;
    leds_apaga(LED_IZQ);
    leds_apaga(LED_DER);
    leds_apaga(LED_FDER);
    leds_apaga(LED_FIZQ);
}

void leds_enciende(int8_t led) {
    if (leds_activados)
        digitalWrite(led, HIGH);
}

void leds_apaga(int8_t led) {
    digitalWrite(led, LOW);
}

void leds_actualiza_valor(int8_t led) {

    leds_lectura0 = analogRead(LED_SENSOR);
    leds_enciende(led);
    delayMicroseconds(50);
    leds_lectura1 = analogRead(LED_SENSOR);
    leds_apaga(led);

    leds_valor_d[led - A0] = leds_lectura0 - leds_lectura1 - leds_valor[led - A0];
    leds_valor[led - A0] = leds_lectura0 - leds_lectura1;
    leds_valor_encendido[led - A0] = leds_lectura1;
    leds_valor_apagado[led - A0] = leds_lectura0;
}

int16_t leds_get_valor(int8_t led) {
    return leds_valor[led-A0];
}

int16_t leds_get_valor_apagado(int8_t led) {
    return leds_valor_apagado[led-A0];
}

int16_t leds_get_valor_encendido(int8_t led) {
    return leds_valor_encendido[led-A0];
}

int16_t leds_get_valor_d(int8_t led) {
    return leds_valor_d[led-A0];
}

int16_t leds_get_desvio_centro() {
    if (leds_pared_izquierda() and leds_pared_derecha()) {
        return leds_valor[LED_IZQ - A0] - leds_valor[LED_DER - A0];
    } else if (leds_pared_izquierda()) {
        return leds_valor[LED_IZQ - A0] - 150;
    } else if (leds_pared_derecha()) {
        // return 30 - leds_valor[LED_DER - A0];
        return 0;
    } else
        return 0;
}

bool leds_pared_enfrente() {
    return leds_valor[LED_FDER - A0] > 9 or leds_valor[LED_FIZQ - A0] > 9; // TODO
}

bool leds_pared_izquierda() {
    return (leds_valor[LED_IZQ - A0] > 5); // TODO
}

bool leds_pared_derecha() {
    return (leds_valor[LED_DER - A0] > 5); // TODO
}

float leds_distancia_frontal() { // TODO: hallar curva caracteristica de los sensores
    if (leds_valor[LED_FDER - A0] + leds_valor[LED_FIZQ - A0] > 800) return 0.01;
    if (leds_valor[LED_FDER - A0] + leds_valor[LED_FIZQ - A0] > 700) return 0.015;
    if (leds_valor[LED_FDER - A0] + leds_valor[LED_FIZQ - A0] > 600) return 0.02;
    if (leds_valor[LED_FDER - A0] + leds_valor[LED_FIZQ - A0] > 500) return 0.035;
    if (leds_valor[LED_FDER - A0] + leds_valor[LED_FIZQ - A0] > 400) return 0.05;
    if (leds_valor[LED_FDER - A0] + leds_valor[LED_FIZQ - A0] > 300) return 0.07;
    if (leds_valor[LED_FDER - A0] + leds_valor[LED_FIZQ - A0] > 200) return 0.08;
    return 0.10;
}
