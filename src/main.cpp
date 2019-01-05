#include <Arduino.h>
#include <motores.h>
#include <bateria.h>
#include <encoders.h>
#include <timer1.h>

volatile uint32_t ticks = 0;

volatile uint16_t max_tcnt1 = 0;

ISR (TIMER1_COMPA_vect) {

    ticks = encoders_get_ticks_entre_saltos_left();
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

    for (pwm = 160; pwm <= 200; pwm+=1) {
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
    delay(600000);

}
