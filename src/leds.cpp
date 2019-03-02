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
    return leds_valor[LED_IZQ - A0] - leds_valor[LED_DER - A0];
}

bool leds_pared_enfrente() {
    return leds_valor[LED_FDER - A0] + leds_valor[LED_FIZQ - A0] > 60; // TODO
}

bool leds_pared_izquierda() {
    return (leds_valor[LED_IZQ - A0] > 5); // TODO
}

bool leds_pared_derecha() {
    return (leds_valor[LED_DER - A0] > 5); // TODO
}
