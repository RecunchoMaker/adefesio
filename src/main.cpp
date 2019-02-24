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

ISR (TIMER1_COMPA_vect) {


    switch (timer1_get_estado()) {
        case 0: 
                leds_actualiza_valor(LED_FDER);
                leds_enciende(LED_IZQ);
                break;
        case 1: 
                leds_actualiza_valor(LED_IZQ);
                leds_enciende(LED_DER);
                break;
        case 2: 
                leds_actualiza_valor(LED_DER);
                leds_enciende(LED_FIZQ);
                break;
        case 3: 
                leds_actualiza_valor(LED_FIZQ);
                leds_enciende(LED_FDER);
                encoders_calcula_velocidad();
                encoders_reset_posicion();
                motores_actualiza_velocidad();
                robot_control();
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

    sei();
}

void loop() {

    comando_prompt();

    while (!comando_get_go()) {
        comando_lee_serial();
    }
    Serial.println("GO!");
    leds_activa();
    robot_init();

    robot_siguiente_accion();


    //while (1) {
    while (robot_get_accion() > 0) {
        Serial.print(robot_get_accion());
        Serial.print(" ");
        /*
        Serial.print(timer1_get_cuenta());
        Serial.print(" ");
        Serial.print(encoders_get_velocidad_left(),6);
        Serial.print(" ");
        Serial.print(encoders_get_velocidad_right(),6);
        Serial.print(" ");
        Serial.print(motores_get_velocidad_lineal_objetivo(),6);
        Serial.print(" ");
        */
        Serial.print(encoders_get_posicion_total());
        Serial.print(" ");
        /*
        /Serial.print(encoders_get_posicion_total() * LONGITUD_PASO_ENCODER/(timer1_get_cuenta() * PERIODO_TIMER),5);
        Serial.print(" ");
        Serial.print(motores_get_aceleracion_lineal(),5);
        Serial.println();
        */
        Serial.print(leds_get_valor(LED_IZQ));
        Serial.print(" \t");
        Serial.print(leds_get_valor(LED_FIZQ));
        Serial.print(" \t");
        Serial.print(leds_get_valor(LED_FDER));
        Serial.print(" \t");
        Serial.print(leds_get_valor(LED_DER));
        Serial.println();
    }

    //for (int i = 0; i< 10000; i++) {
    /*
    while (1) {
        Serial.print(encoders_get_posicion_total_left());
        Serial.print(" \t");
        Serial.print(encoders_get_posicion_total_right());
        Serial.print(" \t - ");
        Serial.print(leds_get_valor(LED_IZQ));
        Serial.print(" \t");
        Serial.print(leds_get_valor(LED_DER));
        Serial.print(" \t");
        Serial.print(leds_get_valor(LED_FIZQ));
        Serial.print(" \t");
        Serial.print(leds_get_valor(LED_FDER));
        Serial.println();
    }
    */
}
