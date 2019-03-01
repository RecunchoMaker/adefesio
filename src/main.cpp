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
#include <laberinto.h>

ISR (TIMER1_COMPA_vect) {


    switch (timer1_get_estado()) {
        case 0: 
//                leds_actualiza_valor(LED_FDER);
                leds_enciende(LED_IZQ);
                break;
        case 1: 
                leds_actualiza_valor(LED_IZQ);
//                leds_enciende(LED_DER);
                break;
        case 2: 
//                leds_actualiza_valor(LED_DER);
//                leds_enciende(LED_FIZQ);
                break;
        case 3: 
//                leds_actualiza_valor(LED_FIZQ);
//                leds_enciende(LED_FDER);
//                encoders_calcula_velocidad();
//                encoders_reset_posicion();
//                motores_actualiza_velocidad();
//                robot_control();
                break;
    }

    // motores_actualiza_angulo();

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
    laberinto_init();

    laberinto_print();

    sei();
}

void loop() {

    comando_prompt();
    leds_activa();

    while (!comando_get_go()) {
        comando_lee_serial();
    }
    Serial.println("GO!");


    //robot_inicia_exploracion();

    //while (1) {
    motores_set_potencia(0.5, 0);
    while (encoders_get_posicion_total() < 1000) {
    //while (robot_get_estado() != ROBOT_PARADO) {
        Serial.print(encoders_get_posicion_total());
        Serial.print(" ");
        // Serial.print(leds_pared_enfrente() ? " | " : "   ");
        Serial.print(leds_get_valor(LED_IZQ));
        Serial.print(" \t");
        Serial.print(leds_get_valor_apagado(LED_IZQ));
        Serial.print(" \t");
        Serial.print(leds_get_valor_encendido(LED_IZQ));
        Serial.println();
    }
    motores_set_potencia(0.0, 0.25);
    while (encoders_get_posicion_total() < 2000) {
        Serial.print(encoders_get_posicion_total());
        Serial.print(" ");
        // Serial.print(leds_pared_enfrente() ? " | " : "   ");
        Serial.print(leds_get_valor(LED_IZQ));
        Serial.print(" \t");
        Serial.print(leds_get_valor_apagado(LED_IZQ));
        Serial.print(" \t");
        Serial.print(leds_get_valor_encendido(LED_IZQ));
        Serial.println();
    }
    motores_set_potencia(0.4, 0.4);
    while (encoders_get_posicion_total() < 4000) {
    //while (robot_get_estado() != ROBOT_PARADO) {
        Serial.print(encoders_get_posicion_total());
        Serial.print(" ");
        // Serial.print(leds_pared_enfrente() ? " | " : "   ");
        Serial.print(leds_get_valor(LED_IZQ));
        Serial.print(" \t");
        Serial.print(leds_get_valor_apagado(LED_IZQ));
        Serial.print(" \t");
        Serial.print(leds_get_valor_encendido(LED_IZQ));
        Serial.println();
    }
    motores_set_potencia(0.0, 0.0);
    while (1) {
    //while (robot_get_estado() != ROBOT_PARADO) {
        Serial.print(encoders_get_posicion_total());
        Serial.print(" ");
        // Serial.print(leds_pared_enfrente() ? " | " : "   ");
        Serial.print(leds_get_valor(LED_IZQ));
        Serial.print(" \t");
        Serial.print(leds_get_valor_apagado(LED_IZQ));
        Serial.print(" \t");
        Serial.print(leds_get_valor_encendido(LED_IZQ));
        Serial.println();
    }

    Serial.println("fin");
}
