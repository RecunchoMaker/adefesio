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
    flood_init(CASILLA_INICIAL, CASILLA_SOLUCION);
    camino_init();
    laberinto_print();

    pinMode(A5, INPUT_PULLUP);
    pinMode(A6, INPUT_PULLUP);

}

void loop() {

    comando_prompt();

    robot_init();
    camino_init();

    while (!comando_get_go()) {
        comando_lee_serial();
        if (leds_get_leds_activados()) {
            log_casilla_pasos_leds();
            //log_variables_trayectoria();
        }
    }

    bateria_watchdog();

    leds_activa();
    delayMicroseconds(32000); // un margen para actualizar los leds
    log_leds();

    robot_inicia_exploracion();

    while (robot_get_estado() != PARADO) {

        log_casilla_pasos_leds();
        //log_leds_distancias();
        //
        /*
            Serial.print("# ");
            Serial.print(motores_get_velocidad_lineal_objetivo());
            Serial.print(" ");
            Serial.print((leds_get_distancia(LED_FDER) + leds_get_distancia(LED_FIZQ)) / 2.0,9);
            Serial.print(" ");
            Serial.print(encoders_get_posicion_total_left());
            Serial.print(" ");
            Serial.print(encoders_get_posicion_total_right());
            Serial.print(" ");

        Serial.print(ACCION_V0 + ( (  (leds_get_distancia(LED_FIZQ) + leds_get_distancia(LED_FDER) / 2) - 0.02 ) * (0.09 / (ACCION_VE - ACCION_V0))), 9);
            Serial.println();
        */
        /*
        Serial.print(leds_get_distancia(LED_DER),9);
        Serial.print(" ");
        Serial.print(leds_get_distancia_d(LED_DER),9);
        Serial.print(" ");
        Serial.print(robot_get_angulo_desvio(),9);
        Serial.print(" ");
        Serial.print(motores_get_radio(),9);
        Serial.print(" ");
        Serial.print(-leds_get_distancia_kalman(LED_IZQ) - (ANCHURA_ROBOT / 2.0) + (LABERINTO_LONGITUD_CASILLA/2.0),9);
        Serial.print(" ");
        Serial.print(motores_get_kp_pasillo1() * (-leds_get_distancia_kalman(LED_IZQ) - (ANCHURA_ROBOT / 2.0) + (LABERINTO_LONGITUD_CASILLA/2.0)),9);
        Serial.print(":(2):");
        Serial.print( motores_get_kp_pasillo2() * leds_get_distancia_d(LED_DER) * motores_get_velocidad_lineal_objetivo(), 9);
        Serial.println("");
        */
    }
    
    leds_desactiva();
    laberinto_print();
    Serial.println("fin");
}
