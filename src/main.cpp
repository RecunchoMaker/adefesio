#include <Arduino.h>
#include <motores.h>
#include <bateria.h>
#include <encoders.h>

void setup() {
    Serial.begin(115200);
    bateria_init();
    motores_init();
    encoders_init();
    bateria_muestra_nivel();
}

void loop() {
    delay(5000);
    encoders_reset_posicion();
    Serial.println("Avanzando durante 5 segundos");
    motores_set_pwm(100, 100);
    delay(5000);
    motores_set_pwm(0,0);
    Serial.print(encoders_get_posicion_left());
    Serial.print(" ");
    Serial.print(encoders_get_posicion_right());
    Serial.println(" ");
}
