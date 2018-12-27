#include <Arduino.h>

void setup() {
    Serial.begin(115200);
}

void loop() {
    Serial.println("Hola mundo");
    delay(1000);
}
