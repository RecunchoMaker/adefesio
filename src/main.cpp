#include <Arduino.h>
#include <settings.h>
#include <motores.h>
#include <bateria.h>
#include <encoders.h>
#include <timer1.h>

ISR (TIMER1_COMPA_vect) {

    encoders_calcula_velocidad();
    //encoders_calcula_velocidad_angular();
    //motores_actualizar_velocidad();
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

// 0.13 salteado
//
float speed = 0.30;
int16_t pwm = 50;

void loop() {

    /*
     * prueba set velocidad
     */

    motores_set_pwm(pwm, pwm);
    while (encoders_get_posicion_total_right() * LONGITUD_PASO_ENCODER < 1.0) {
#ifdef ENCODERS_LOG_ESTADO
        cli();
        encoders_log_estado();
        sei();
        delay(100);
#endif
    }
    pwm += 10;
    encoders_reset_posicion_total();

    if (pwm >= 200) {
        motores_set_pwm(0,0);
        while(1);
    }

}
