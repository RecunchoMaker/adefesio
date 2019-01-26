#include <Arduino.h>
#include <settings.h>
#include <robot.h>
#include <motores.h>
#include <bateria.h>
#include <encoders.h>
#include <timer1.h>
#include <log.h>
#include <comando.h>

volatile uint8_t max_tcnt1=0;

volatile uint8_t min_tcnt1=255;

ISR (TIMER1_COMPA_vect) {

    encoders_calcula_velocidad();
    motores_actualiza_velocidad();
    motores_actualiza_angulo();
    robot_actualiza_posicion();
    encoders_reset_posicion();

}

void setup() {
    Serial.begin(115200);
    bateria_init();
    motores_init(bateria_get_voltaje());
    encoders_init();
    comando_init();
    bateria_muestra_nivel();

    timer1_init(PERIODO_TIMER, 1);
#ifdef ENCODERS_LOG_ESTADO
    encoders_log_estado_cabecera();
#endif
#ifdef ROBOT_LOG_ESTADO
    robot_log_estado_cabecera();
#endif

    sei();
    motores_set_velocidad(0, 0);
    robot_set_posicion(0,0);
    robot_set_orientacion(ESTE);
}

uint8_t ori = OESTE;
float pos = 0.40;

void loop() {

    comando_prompt();
    while (!comando_go()) {
        comando_lee_serial();
    }
    Serial.println("GO!");

    /* Pruebas de aceleracion y deceleracion 
    motores_set_velocidad(0.2, 0);
    delay(1000);
    log_start();
    motores_set_velocidad(0.1, 0);
    while (robot_get_posicion_x() <= 0.5) {
        log_print();
    }
    robot_parar();

    while(1);
    */

    robot_ir_a(pos, 0, RECTO);
#ifdef MOTORES_LOG_PID
    log_start();
#endif
    while (ori == OESTE and robot_get_posicion_x() <= 0.40 or ori == ESTE and robot_get_posicion_x() >=0 ) {
#ifdef ROBOT_LOG_ESTADO
        robot_log_estado();
        delay(100);
#endif
#ifdef ENCODERS_LOG_ESTADO
        encoders_log_estado();
        delay(100);
#endif
#ifdef MOTORES_LOG_PID
       log_print();
#endif
    }

    /*

    pos = (pos == 0.40?0.0:0.40);

    //log_start();
    robot_gira(-1);
    while (robot_get_orientacion() != ori) {
#ifdef ROBOT_LOG_ESTADO
        robot_log_estado();
        delay(100);
#endif
#ifdef ENCODERS_LOG_ESTADO
        encoders_log_estado();
        delay(100);
#endif
    }


    ori = (ori == OESTE?ESTE:OESTE);

    */
    motores_set_potencia(0,0);
    motores_set_velocidad(0,0);
}
