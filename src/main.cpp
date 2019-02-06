#include <Arduino.h>
#include <settings.h>
#include <robot.h>
#include <motores.h>
#include <bateria.h>
#include <encoders.h>
#include <timer1.h>
#include <log.h>
#include <comando.h>
#include <robot.h>

volatile uint8_t max_tcnt1=0;
volatile uint8_t min_tcnt1=255;

volatile uint32_t aux_1;
volatile uint32_t aux_2;

volatile float aux_1f=0;

ISR (TIMER1_COMPA_vect) {

    encoders_calcula_velocidad();
    encoders_reset_posicion();
    motores_actualiza_velocidad();
    // motores_actualiza_angulo();
    robot_control();

    /*
     * Bucle inutil, para evaluar el impacto del tiempo
     * de proceso del timer en el calculo de la velocidad
     */
    /*
    for (aux_1 = 0; aux_1 < 300; aux_1++) {
        aux_1f += 0/(aux_1);
    }
    if (aux_1f == aux_1 * 1.0) // nunca ocurre
        timer1_incrementa_cuenta();

    */

    timer1_incrementa_cuenta();

}

void setup() {

    Serial.begin(115200);
    cli();

    bateria_init();
    motores_init(bateria_get_voltaje());
    encoders_init();
    comando_init();
    bateria_muestra_nivel();
    timer1_init(PERIODO_TIMER, 1);
    robot_init();

    sei();
}

void loop() {

    comando_prompt();

    while (!comando_go()) {
        comando_lee_serial();
    }
    Serial.println("GO!");

    /*
    for (float p = 0.20; p<=0.20; p+=1.04) {
        Serial.println(p,5);

        motores_set_aceleracion_lineal(0);
        motores_set_velocidad_lineal_objetivo(p);
        motores_set_radio(99999);

        encoders_reset_posicion_total();
        while (encoders_get_posicion_total() < 517); // estabiliza 20cm
        encoders_reset_posicion_total();
        timer1_reset_cuenta();

        while (encoders_get_posicion_total() < 2070) { // 80cm hasta un metro
            
            // Serial.print(encoders_get_tcnt1_anterior_right());
            // Serial.print(" ");
            // Serial.print(encoders_get_ultimo_tcnt1_right());
            Serial.print(timer1_get_cuenta());
            Serial.print(" ");
            Serial.print(encoders_get_velocidad_left(),6);
            Serial.print(" ");
            Serial.print(encoders_get_velocidad_right(),6);
            Serial.print(" ");
            Serial.print(motores_get_pwm_left());
            Serial.print(" ");
            Serial.print(motores_get_pwm_right());
            Serial.print(" ");
            Serial.print(motores_get_velocidad_lineal_objetivo(),2);
            Serial.print(" ");
            Serial.print(encoders_get_posicion_total() * LONGITUD_PASO_ENCODER/(timer1_get_cuenta() * PERIODO_TIMER),5);
            Serial.print(" ");
            Serial.print(encoders_get_posicion_total());
            Serial.print(" ");
            Serial.print(encoders_get_ticks_sin_actualizar_left());
            Serial.println();
        }

    }
    Serial.println("FIN");
    motores_parar();
    */

    robot_siguiente_accion();

    while (robot_get_accion() > 0) {
        Serial.print(robot_get_accion());
        Serial.print(" ");
        Serial.print(timer1_get_cuenta());
        Serial.print(" ");
        Serial.print(encoders_get_velocidad_left(),6);
        Serial.print(" ");
        Serial.print(encoders_get_velocidad_right(),6);
        Serial.print(" ");
        Serial.print(motores_get_velocidad_lineal_objetivo(),6);
        Serial.print(" ");
        Serial.print(encoders_get_posicion_total());
        Serial.println();
    }
}
