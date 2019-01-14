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
    motores_actualizar_velocidad();
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

// 0.13 salteado
//
float speed = 0.30;

void loop() {

    /*
     * prueba set velocidad
     */

    motores_set_velocidad(speed, speed);
    while (encoders_get_posicion_total_right() * LONGITUD_PASO_ENCODER < 2.9) {
        Serial.print(speed);
        Serial.print(" vel:");
        Serial.print(motores_get_velocidad_actual());
        Serial.print("-");
#ifdef ENCODERS_LOG_ESTADO
        cli();
        encoders_log_estado();
        sei();
#endif
        delay(100);
    }

    encoders_reset_posicion_total();

    if (speed == 0.30)
        speed = 0.12;
    else
        speed = 0.30;


    // while(1);

}
