#include <Arduino.h>
#include <motores.h>
#include <bateria.h>
#include <encoders.h>
#include <timer1.h>

volatile uint16_t contador = 0;
uint32_t microsegundos = 0;

ISR (TIMER1_COMPA_vect) {
    contador++;
}

void setup() {
    Serial.begin(115200);
    bateria_init();
    motores_init();
    encoders_init();
    bateria_muestra_nivel();
    timer1_init(0.001, 1);
    sei();
    microsegundos = micros();
}


void loop() {
    if (contador == 1023) {
        Serial.println("1023!");
    }
    cli();
    if (contador > 1000) {
        Serial.println(micros() - microsegundos);
        contador = 0;
        microsegundos = micros();
    }
    sei();
}
