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

volatile uint8_t max_tcnt1=0;

volatile uint8_t min_tcnt1=255;

ISR (TIMER1_COMPA_vect) {

    encoders_calcula_velocidad();
    // motores_actualiza_velocidad();
    // motores_actualiza_angulo();
    robot_control();
    encoders_reset_posicion();
    timer1_incrementa_cuenta();

}

void setup() {

    Serial.begin(115200);
    cli();

    bateria_init();
    motores_init(bateria_get_voltaje());
    encoders_init();
    comando_init();
    bateria_muestra_nivel();
    timer1_init(PERIODO_TIMER, 1);
    robot_init();

    sei();
}

void loop() {

    comando_prompt();

    while (!comando_go()) {
        comando_lee_serial();
    }
    Serial.println("GO!");
    robot_siguiente_accion();

}
