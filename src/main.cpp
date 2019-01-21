#include <Arduino.h>
#include <settings.h>
#include <motores.h>
#include <bateria.h>
#include <encoders.h>
#include <timer1.h>

volatile uint8_t max_tcnt1=0;

volatile uint8_t min_tcnt1=255;

ISR (TIMER1_COMPA_vect) {

    encoders_calcula_velocidad();
    motores_actualiza_velocidad();
    motores_actualiza_angulo();
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

    sei();
    motores_set_velocidad(0, 0);
}

void loop() {

    // motores_set_velocidad(speed, speed / 0.20);

    motores_set_velocidad(0.0, PI/4.0);
    // while (encoders_get_posicion_total_left() * LONGITUD_PASO_ENCODER < 20 *speed) {
    while (motores_get_angulo_actual() < PI) {
        if (motores_get_angulo_actual() > PI-0.3)
            motores_set_velocidad(0, PI/4.0);
        else if (motores_get_angulo_actual() > 0.1)
            motores_set_velocidad(0.0, PI/3.5);
#ifdef ENCODERS_LOG_ESTADO
        cli();
        encoders_log_estado();
        sei();
        delay(100);
#endif
    }
    cli();
    motores_set_pwm(0,0);
    while(1);
}
