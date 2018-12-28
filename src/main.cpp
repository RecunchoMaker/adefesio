#include <Arduino.h>
#include <motores.h>

void setup() {
    Serial.begin(115200);
    motores_init();
}

void loop() {
    for (int16_t i = -200; i < 200; i++) {
        Serial.println(i);
        motores_set_pwm(i, -i);
        delay(100);
    }
    delay(1000);
}
