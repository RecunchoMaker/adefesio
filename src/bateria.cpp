/**
 * @file bateria.cpp
 * @brief Funciones para controlar la carga de la bateria
 *
 * Suponemos que el polo positivo de la batería está conectado a una entrada
 * analógica del procesador (probablemente con un divisor de voltaje)
 *
 * Uso: se llama una única vez a bateria_init() para inicializar los pines.
 * Posteriormente se puede llamar en cualquier momento a bateria_muestra_nivel()
 * o a bateria_watchdog(). Si la carga esté por debajo del nivel
 * crítico, se detendrá la ejecución. En el primer caso se escribe en el puerto
 * serie una descripción textual del valor de la carga. El segundo es silencioso.
 *
 * @see BATERIA_VOLTAJE_PIN
 * @see BATERIA_LED_PIN
 * @see BATERIA_VOLTAJE_USB
 * @see https://recunchomaker.github.io/adefesio/control-de-bateria/
 *
 */

#include <bateria.h>

/**
 * @brief Inicializa pins y comprueba carga
 *
 * Se inicializan el pin analógico de lectura y el pin que se utiliza como salida
 * para el led en caso de tener que reportar batería descargada. 
 *
 * @note Llama a bateria_watchdog() después de la inicialización
 */

void bateria_init(void) {
    pinMode(BATERIA_VOLTAJE_PIN, INPUT);
    pinMode(BATERIA_LED_PIN, OUTPUT);
    bateria_watchdog();
}


/**
 * @brief Devuelve la carga de la batería
 *
 * El voltaje entra en el pin en el punto medio de un divisor de voltaje
 * de 10K/10K, y el rango dinámico del puerto de entrada es de 5v. La
 * función normaliza el rango 0-1024 del puerto a los valores 0v-10v.
 *
 * @return Valor en voltios de la carga de la bateria
 */
float bateria_get_voltaje() {
    return 10.0 * analogRead(BATERIA_VOLTAJE_PIN) / 1024.0;
}


/**
 * @brief Escribe en el puerto serie una descrición en texto la carga de la batería
 *
 * @note Se llama a bateria_watchdog() para detener la ejecución en caso de que
 * la batería esté agotada.
 */
void bateria_muestra_nivel() {
    float bv = bateria_get_voltaje();
    if (bv < BATERIA_VOLTAJE_USB) {
        Serial.println("USB");
    }
    else {
        Serial.print("Bateria: ");
        Serial.print(bv, 2);
        Serial.println("v");
        bateria_watchdog();
    }
}


/**
 * @brief Devuelve si la batería está o no agotada
 *
 * @note En caso de que el umbral esté por debajo de BATERIA_VOLTAJE_USB,
 * se supone que el robot está conectado por USB y la función devuelve
 * siempre _false_.
 */
bool _bateria_agotada() {
    float bv = bateria_get_voltaje();
    if (bv < BATERIA_VOLTAJE_USB)
        return false;
    return bv <= BATERIA_VOLTAJE_UMBRAL;
}


/**
 * @brief Reporta si la batería está agotada
 *
 * En caso de que la batería esté agotada, se muestra
 * el mensaje por el puerto serie y se parpadea de forma
 * indefinida el led correspondiente.
 */
void bateria_watchdog() {
    if (_bateria_agotada()) {
        Serial.println("Bateria agotada!");
        while (1) {
            digitalWrite(BATERIA_LED_PIN, HIGH);
            delay(300);
            digitalWrite(BATERIA_LED_PIN, LOW);
            delay(700);
        }
    }
}
