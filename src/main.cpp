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
    camino_init();
    laberinto_print();

    pinMode(A5, INPUT_PULLUP);
    pinMode(A6, INPUT_PULLUP);

}

void loop() {

    comando_prompt();

    robot_init();
    camino_init();


    //float dd = 0.00034;
    //float vv = 0.30;
    //Serial.print("calculo: ");
    //Serial.println(dd * PERIODO_CICLO / vv, 9);
    //Serial.println((dd / vv ) * PERIODO_CICLO, 9);

    for (int8_t i = 0; i< 5;i++)  {
        Serial.println("Anado recto");
        camino_anadir_paso_recto();
    }


    while (!comando_get_go()) {
        comando_lee_serial();
        if (leds_get_leds_activados()) {
            //log_casilla_pasos_leds();
            //log_variables_trayectoria();
            Serial.print(robot_get_desvio_centro(),5);
            Serial.print("\t");
            Serial.print(robot_get_desvio_centro() * 57.3,5);
            Serial.print("\t");
            Serial.print(leds_get_distancia_kalman(LED_IZQ),5);
            Serial.print("\t");
            Serial.print((LABERINTO_LONGITUD_CASILLA/2.0) - leds_get_distancia_kalman(LED_IZQ) - LEDS_DISTANCIA, 5);
            Serial.print("\t");
            Serial.print(leds_get_distancia_d(LED_IZQ),5);

            Serial.println();
                //desvio += 0.2 / ((LABERINTO_LONGITUD_CASILLA/2.0) - leds_get_distancia_kalman(LED_IZQ) - LEDS_DISTANCIA);
        }
    }

    leds_activa();
    delayMicroseconds(32000); // un margen para actualizar los leds

    robot_inicia_exploracion();
    sei();

    while (robot_get_estado() != PARADO) {

        //log_casilla_pasos_leds();
        log_variables_trayectoria();

    }
    
    leds_desactiva();
    laberinto_print();
    Serial.println("fin");
}
