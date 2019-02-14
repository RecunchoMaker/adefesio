#include <Arduino.h>
#include <leds.h>

bool leds_activados = false;
int16_t leds_valor[4];

void leds_init() {

    pinMode(LED_IZQ, OUTPUT);
    pinMode(SENSOR_IZQ, INPUT);

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

void leds_actualiza_valor(int8_t sensor) {
    leds_valor[sensor-14] = analogRead(sensor);
}

int16_t leds_get_valor(int8_t sensor) {
    return leds_valor[sensor-14];
}
