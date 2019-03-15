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
    flood_init(CASILLA_INICIAL, CASILLA_SOLUCION);
    laberinto_print();

    sei();
}

void loop() {

    comando_prompt();
    leds_activa();

    while (!comando_get_go()) {
        comando_lee_serial();
        log_leds();
    }
    Serial.println("GO!");


    leds_activa();
    robot_inicia_exploracion();

    uint8_t casilla = 0;

    int32_t aux;

    while (1) {

        if (robot_get_estado() == EXPLORANDO_INICIAL and casilla != robot_get_casilla()) {
            casilla = robot_get_casilla();
            laberinto_print();
        }

        if (robot_get_casilla() != 99) {
            if (robot_get_casilla() == 99) {
                //laberinto_print();
                
                Serial.print(encoders_get_posicion_total());
                Serial.print(" \t");
                log_leds();
                Serial.print(motores_get_velocidad_lineal_objetivo());
                Serial.print(" \t");
                Serial.print(robot_get_casilla());
                Serial.print(" \t");
                Serial.print(robot_get_casilla_offset(),4);
                Serial.print(" \t");
                Serial.print(aux * LONGITUD_PASO_ENCODER + 0.09, 6);
                Serial.print(" \t");
                //Serial.print(robot_get_casilla_offset() + aux_casillas * 0.18,6);
                //Serial.print(" \t");
                //Serial.print(aux_casillas);
                //Serial.print(" \t");
                Serial.print(motores_get_velocidad_lineal_objetivo());
                Serial.print(" \t");
                Serial.print(  (encoders_get_posicion_total_right() - encoders_get_posicion_total_left()) / 2.0,3);

                Serial.println();

            }

        }

    }
    
    motores_set_potencia(0.0, 0);
    leds_desactiva();
    Serial.println("fin");
}
