#include <Arduino.h>
#include <settings.h>
#include <robot.h>
#include <motores.h>
#include <bateria.h>
#include <encoders.h>
#include <timer1.h>
#include <log.h>
#include <comando.h>
#include <robot.h>
#include <leds.h>

volatile uint8_t max_tcnt1=0;
volatile uint8_t min_tcnt1=255;

volatile uint32_t aux_1;
volatile uint32_t aux_2;

volatile float aux_1f=0;

volatile uint8_t timer_estado = 0;

ISR (TIMER1_COMPA_vect) {

    timer_estado++;

    switch (timer_estado & B00000011) {
        case 0: digitalWrite(LED_IZQ, HIGH);
                break;
        case 1: digitalWrite(LED_IZQ, LOW);
                break;
        case 2: 
                break;
        case 3: 
                break;
    }
    encoders_calcula_velocidad();
    encoders_reset_posicion();
    motores_actualiza_velocidad();
    // motores_actualiza_angulo();
    robot_control();

    /*
    aux_1f = TCNT1;
    digitalWrite(LED_IZQ, HIGH);
    while (TCNT1 < aux_1f + 500);
    digitalWrite(LED_IZQ, LOW);
    */

    /*
     * Bucle inutil, para evaluar el impacto del tiempo
     * de proceso del timer en el calculo de la velocidad
     */
    /*
    for (aux_1 = 0; aux_1 < 300; aux_1++) {
        aux_1f += 0/(aux_1);
    }
    if (aux_1f == aux_1 * 1.0) // nunca ocurre
        timer1_incrementa_cuenta();

    */

    timer1_incrementa_cuenta();

}

void setup() {

    Serial.begin(115200);
    cli();

    bateria_init();
    motores_init(bateria_get_voltaje());
    encoders_init();
    leds_init();
    comando_init();
    bateria_muestra_nivel();
    timer1_init(PERIODO_TIMER, 1);
    robot_init();

    sei();
}

void loop() {

    comando_prompt();

    while (!comando_go()) {
        comando_lee_serial();
    }
    Serial.println("GO!");

    for (int i = 0; i<1000; i++) {
        Serial.println(aux_1f);
    }


}
