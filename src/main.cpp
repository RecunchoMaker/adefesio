#include <Arduino.h>
#include <motores.h>
#include <bateria.h>
#include <encoders.h>
#include <timer1.h>

volatile uint32_t ticks = 0;

volatile uint16_t max_tcnt1 = 0;

volatile uint8_t log_buffer_idx = 0;
volatile uint16_t log_buffer[256];

ISR (TIMER1_COMPA_vect) {

    ticks = encoders_get_ticks_entre_saltos_left();

    /*
    log_buffer[log_buffer_idx++] = ticks;
    if (log_buffer_idx == 255) 
        for (log_buffer_idx = 0; log_buffer_idx < 255; log_buffer_idx++)
            Serial.println(log_buffer[log_buffer_idx]);
    */

    encoders_reset_posicion();

    /* control de maximo TCNT1
    if (TCNT1 > max_tcnt1) {
        max_tcnt1 = TCNT1;
        Serial.print("max :");
        Serial.println(max_tcnt1);
    }
    */
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

uint8_t pwm = 100;
void loop() {
    delay(100);

    for (pwm = 50; pwm <= 240; pwm+=10) {
        Serial.print("pwm = ");
        Serial.println(pwm);
        motores_set_pwm(pwm,pwm);
        
        for (int i = 0; i< 5; i++) {
            
            cli();
            encoders_print();
            sei();

            delay(200);
        }
    }
    motores_set_pwm(0,0);
    while(1);

}
