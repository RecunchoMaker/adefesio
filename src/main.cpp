#include <Arduino.h>
#include <motores.h>
#include <bateria.h>

void setup() {
    Serial.begin(115200);
    bateria_init();
    motores_init();

}

void loop() {
    delay(1000);
    bateria_muestra_nivel();
}
