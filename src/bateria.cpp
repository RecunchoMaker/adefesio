#include <bateria.h>

void bateria_init(void) {
    pinMode(BATERIA_VOLTAJE_PIN, INPUT);
    pinMode(BATERIA_LED_PIN, OUTPUT);
    bateria_watchdog();
}

float bateria_get_voltaje() {
    return 10.0 * analogRead(BATERIA_VOLTAJE_PIN) / 1024.0;
}

bool bateria_agotada() {
    float bv = bateria_get_voltaje();
    if (bv < BATERIA_VOLTAJE_USB)
        return false;
    return bateria_get_voltaje() <= BATERIA_VOLTAJE_UMBRAL;
}

void bateria_muestra_nivel() {
    float bv = bateria_get_voltaje();
    if (bv < BATERIA_VOLTAJE_USB) {
        // No imprimir
        // Serial.println("USB");
    }
    else {
        Serial.print("Bateria: ");
        Serial.print(bv, 2);
        Serial.println("v");
        bateria_watchdog();
    }
}

void bateria_watchdog() {
    if (bateria_agotada()) {
        while (1) {
            bateria_muestra_nivel();
            Serial.println("Bateria agotada!");
            digitalWrite(BATERIA_LED_PIN, HIGH);
            delay(300);
            digitalWrite(BATERIA_LED_PIN, LOW);
            delay(700);
        }
    }
}
            
