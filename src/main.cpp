#include <Arduino.h>
#include <motores.h>
#include <bateria.h>
#include <encoders.h>
#include <timer1.h>

volatile int16_t pasos_left = 0;
volatile int16_t pasos_right = 0;

volatile int16_t last_pasos_left = 0;
volatile int16_t last_pasos_right = 0;

ISR (TIMER1_COMPA_vect) {

    encoders_calcula_ticks_left();
    encoders_calcula_ticks_right();
    pasos_left += encoders_get_posicion_left();
    pasos_right += encoders_get_posicion_right();
    encoders_reset_posicion();
    motores_actualizar_pwm();
}

void setup() {
    Serial.begin(115200);
    bateria_init();
    motores_init();
    encoders_init();
    bateria_muestra_nivel();

    timer1_init(0.001, 1);
    sei();
}

int32_t ticks = 0;
void loop() {
    delay(100);

#ifdef ENCODERS_LOG_ESTADO
    encoders_log_estado_cabecera();
#endif

    for (ticks = 20000; ticks <= 50000; ticks+=1000) {
        motores_set_ticks(ticks, ticks);
      
        for (int i = 0; i< 2; i++) {
            
            cli();
#ifdef ENCODERS_LOG_ESTADO
            encoders_log_estado();
#endif
            last_pasos_right = pasos_right;
            last_pasos_left = pasos_left;
            sei();


            delay(500);
        }
    }
    motores_set_pwm(0,0);
    while(1);

}
