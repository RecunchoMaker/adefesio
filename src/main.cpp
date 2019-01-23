#include <Arduino.h>
#include <settings.h>
#include <robot.h>
#include <motores.h>
#include <bateria.h>
#include <encoders.h>
#include <timer1.h>
#include <log.h>

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
    motores_init();
    encoders_init();
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
float pos = 1.00;

void loop() {

    robot_ir_a(pos, 0, RECTO);
    log_start();
    while (ori == OESTE and robot_get_posicion_x() <= 1.00 or ori == ESTE and robot_get_posicion_x() >=0 ) {
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

    pos = (pos == 1.00?0.0:1.00);

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

}
