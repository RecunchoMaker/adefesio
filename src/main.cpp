#include <Arduino.h>
#include <settings.h>
#include <motores.h>
#include <bateria.h>
#include <encoders.h>
#include <timer1.h>

ISR (TIMER1_COMPA_vect) {

    encoders_calcula_ticks_left();
    encoders_calcula_ticks_right();
    //encoders_calcula_velocidad_angular();
    encoders_reset_posicion();
    motores_actualizar_pwm();
}

void setup() {
    Serial.begin(115200);
    bateria_init();
    motores_init();
    encoders_init();
    bateria_muestra_nivel();

    timer1_init(FRECUENCIA_TIMER, 1);
#ifdef ENCODERS_LOG_ESTADO
    encoders_log_estado_cabecera();
#endif

    sei();
}

int32_t ticks = 0;
uint8_t pwm = 100;
float diff = 0;

void loop() {


    /*
     * prueba avanzar un metro
    while (encoders_get_posicion_total_right() * LONGITUD_PASO_ENCODER < 1.0) {
        if (encoders_get_posicion_right() * LONGITUD_PASO_ENCODER > 0.90) 
            pwm-=3;
        diff = encoders_get_posicion_left() - encoders_get_posicion_right();
        motores_set_pwm(pwm, pwm + diff);
        delay(100);
        Serial.print(encoders_get_posicion_right() * LONGITUD_PASO_ENCODER);
#ifdef ENCODERS_LOG_ESTADO
        encoders_log_estado();
#endif
    }
     */

    /*
     * prueba set velocidad
     */

    motores_set_velocidad(0.30, 0);
    while (encoders_get_posicion_total_right() * LONGITUD_PASO_ENCODER < 1.3) {
        encoders_log_estado();
        delay(100);
    }


    motores_set_velocidad(0.0, 0);

    Serial.flush();
    Serial.end();
    while(1);

}
