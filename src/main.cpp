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
    flood_init(CASILLA_SOLUCION);
    camino_init();
    laberinto_print();

    pinMode(A5, INPUT_PULLUP);
    pinMode(A6, INPUT_PULLUP);

}

void loop() {

    comando_prompt();

    robot_init();
    camino_init();

    leds_activa();
    //while(true) log_casilla_pasos_leds();
    while (!leds_go() and !comando_get_go()) {
        comando_lee_serial();
        if (leds_get_leds_activados()) {
            //log_casilla_pasos_leds();
        }
    }

    bateria_watchdog();

    log_leds();

    robot_inicia_exploracion();

    while (robot_get_estado() != PARADO) {
        Serial.print(">");
        Serial.print(robot_es_valido_led_izquierdo());
        Serial.println(robot_es_valido_led_derecho());

        log_casilla_pasos_leds();

    }
    laberinto_print();
    Serial.println("encontrada solucion");

    while (!leds_go());

    robot_resuelve();
    
    leds_desactiva();
}
