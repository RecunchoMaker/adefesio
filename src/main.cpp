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
#include <camino.h>
#include <accion.h>

ISR (TIMER1_COMPA_vect) {

#ifdef MOCK
    robot_control();
#else
    switch (timer1_get_estado()) {
        case 0: 
                leds_actualiza_valor(LED_IZQ);
                break;
        case 1: 
                leds_actualiza_valor(LED_FDER);
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
#endif
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
    laberinto_print();

    pinMode(A5, INPUT_PULLUP);
    pinMode(A6, INPUT_PULLUP);

}

void loop() {

    comando_prompt();

    robot_init();
    camino_init();

    leds_activa();
    leds_reset_go();


    while (true) {
#ifdef MOCK
        while (!comando_get_go()) {
#else
        while (!leds_go() and !comando_get_go()) {
#endif
            comando_lee_serial();
            //log_casilla_pasos_leds();
        }
        flood_init(CASILLA_SOLUCION);
        camino_init();

        bateria_watchdog();
        Serial.print("antes");

        robot_empieza();
        while (robot_get_estado() != ESPERANDO_SENAL) {
#ifdef MOCK
            comando_lee_serial();
#endif
        }
    }

    
}
