#include <Arduino.h>
#include <leds.h>

volatile bool leds_activados = false;
volatile int16_t leds_valor[4];

volatile int16_t leds_lectura0;
volatile int16_t leds_lectura1;

volatile uint32_t led_aux;

void leds_init() {

    pinMode(LED_IZQ, OUTPUT);
    pinMode(LED_DER, OUTPUT);
    pinMode(LED_FIZQ, OUTPUT);
    pinMode(LED_FDER, OUTPUT);

    pinMode(LED_SENSOR, INPUT);

    leds_activados = false;
}

void leds_activa() {
    leds_activados = true;
}

void leds_desactiva() {
    leds_activados = false;
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
    leds_apaga(led);
    // sin esperas maxima lectura 230
    delayMicroseconds(15); // maxima lectura 460
    delayMicroseconds(15); // maxima lectura 650
    delayMicroseconds(15); // maxima lectura 710
    delayMicroseconds(15); // maxima lectura 710
    leds_lectura1 = analogRead(LED_SENSOR);

    leds_valor[led - A1] = leds_lectura1 - leds_lectura0;
}

int16_t leds_get_valor(int8_t led) {
    return leds_valor[led-A1];
}
