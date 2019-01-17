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
    //encoders_calcula_velocidad_angular();
    motores_actualizar_velocidad();
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

float speed = 0.07;
int16_t pwm = 50;

void loop() {

    motores_set_velocidad(speed, 3.14159);
    while (encoders_get_posicion_total_right() * LONGITUD_PASO_ENCODER < 5*speed) {
#ifdef ENCODERS_LOG_ESTADO
        cli();
        encoders_log_estado();
        sei();
        delay(100);
#endif
    }
    encoders_reset_posicion_total();

    speed += 0.05;

    if (speed > 0.25) 
    {
        motores_set_pwm(0,0);
        cli();
        while(1);
    }
}
