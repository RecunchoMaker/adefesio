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
#include <flood.h>

volatile int8_t led = LED_FIZQ;
ISR (TIMER1_COMPA_vect) {

    switch (timer1_get_estado()) {
        case 0: 
                leds_actualiza_valor(LED_FDER);
                break;
        case 1: 
                leds_actualiza_valor(LED_IZQ);
                break;
        case 2: 
                leds_actualiza_valor(LED_DER);
                break;
        case 3: 
                leds_actualiza_valor(LED_FIZQ);
                encoders_calcula_velocidad();
                encoders_reset_posicion();
                motores_actualiza_velocidad();
                robot_control();
                break;
    }

    timer1_incrementa_cuenta();

}

void setup() {

    Serial.begin(115200);

    bateria_init();
    motores_init(bateria_get_voltaje());
    encoders_init();
    leds_init();
    comando_init();
    bateria_muestra_nivel();
    timer1_init(PERIODO_TIMER, 1);
    robot_init();
    laberinto_init();
    flood_init(CASILLA_INICIAL, CASILLA_SOLUCION);
    laberinto_print();

    pinMode(A5, INPUT_PULLUP);
    pinMode(A6, INPUT_PULLUP);

    sei();
}

void loop() {

    comando_prompt();

    while (!comando_get_go()) {
        comando_lee_serial();
        if (leds_get_leds_activados()) {
            log_leds();
        }
    }
    leds_activa();

    while((leds_get_valor(LED_DER) < 200 and
        leds_get_valor(LED_IZQ) < 200)) {
            log_leds();
            while(timer1_get_cuenta() < 1000);
            timer1_reset_cuenta();
    }

    Serial.println("GO!");

    robot_inicia_exploracion();

    uint8_t casilla = 0;

    int32_t aux;

    while (1) {

        log_casilla_pasos_leds();

    }
    
    motores_set_potencia(0.0, 0);
    leds_desactiva();
    Serial.println("fin");
}
