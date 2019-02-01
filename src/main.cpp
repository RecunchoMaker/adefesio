#include <Arduino.h>
#include <settings.h>
#include <robot.h>
#include <motores.h>
#include <bateria.h>
#include <encoders.h>
#include <timer1.h>
#include <log.h>
#include <comando.h>

volatile uint8_t max_tcnt1=0;

volatile uint8_t min_tcnt1=255;

ISR (TIMER1_COMPA_vect) {

    encoders_calcula_velocidad();
    motores_actualiza_velocidad();
    motores_actualiza_angulo();
    robot_actualiza_posicion();
    encoders_reset_posicion();
    timer1_incrementa_cuenta();

}

void setup() {
    Serial.begin(115200);
    bateria_init();
    motores_init(bateria_get_voltaje());
    encoders_init();
    comando_init();
    bateria_muestra_nivel();

    timer1_init(PERIODO_TIMER, 1);
#ifdef ENCODERS_LOG_ESTADO
    encoders_log_estado_cabecera();
#endif
#ifdef ROBOT_LOG_ESTADO
    robot_log_estado_cabecera();
#endif

    sei();
    motores_set_velocidad(0, 0);
    robot_set_posicion(0,0);
    robot_set_orientacion(ESTE);
}

uint8_t ori = ESTE;
float distancia;
float parar_en;

void loop() {

    comando_prompt();

    robot_set_posicion(0,0);
    robot_set_orientacion(ESTE);
    motores_set_potencia(0,0);
    motores_set_velocidad(0,0);
    encoders_reset_posicion();
    encoders_reset_posicion_total();

    while (!comando_go()) {
        comando_lee_serial();
    }
    Serial.println("GO!");

    /*
    // avanzo 40 cm.
    distancia = 0.4;
    parar_en = distancia - 
           motores_get_maxima_velocidad_lineal() * motores_get_maxima_velocidad_lineal() /
           (motores_get_maxima_aceleracion_lineal() * 2);
    Serial.print("Recorrrere: ");
    Serial.println(distancia);


    // acelero
    motores_set_aceleracion_lineal(motores_get_maxima_aceleracion_lineal());
    while (motores_get_velocidad_lineal_objetivo_temp() < motores_get_maxima_velocidad_lineal());
    Serial.print("acelero hasta ");
    Serial.println(encoders_get_posicion_total_left() * LONGITUD_PASO_ENCODER);
    
    // mantengo constante
    motores_set_aceleracion_lineal(0);
    while (encoders_get_posicion_total_left() * LONGITUD_PASO_ENCODER < parar_en);
    Serial.print("decelero en ");
    Serial.print(encoders_get_posicion_total_left() * LONGITUD_PASO_ENCODER);
    Serial.print(" - deberia en ");
    Serial.println(parar_en);

    // decelero
    bool aux_bool = false;
    motores_set_aceleracion_lineal(-motores_get_maxima_aceleracion_lineal());
    while (motores_get_velocidad_lineal_objetivo_temp() > 0) {
#ifdef MOTORES_LOG_PID
        if (motores_get_velocidad_lineal_objetivo_temp() < 0.20 and aux_bool == false) {
            log_start();
            aux_bool = true;
        }
        else if (aux_bool)
            log_print();
#endif
    }

    Serial.print("paro en ");
    Serial.println(encoders_get_posicion_total_left() * LONGITUD_PASO_ENCODER, 5);
    motores_set_aceleracion_lineal(0);
    for (uint8_t aux = 0; aux<100; aux++) log_print();

    */

    /*
    // Girando sobre si mismo
    //
    distancia = PI;  // en radianes

    // calculo en radianes
    parar_en = distancia - 
           motores_get_maxima_velocidad_angular() * motores_get_maxima_velocidad_angular() /
           (motores_get_maxima_aceleracion_angular() * 2);

    Serial.println("parar en radianes: ");
    Serial.println(parar_en);
    // pasar a metros
    parar_en = parar_en * (DISTANCIA_ENTRE_RUEDAS/2);

    motores_set_aceleracion_angular(motores_get_maxima_aceleracion_angular());

    log_start();
    while (motores_get_velocidad_angular_objetivo_temp() < motores_get_maxima_velocidad_angular()) {
#ifdef MOTORES_LOG_PID
        log_print();
#endif
    }

    Serial.print("acelero hasta ");
    Serial.println(encoders_get_posicion_total_left() * LONGITUD_PASO_ENCODER);
    Serial.print("la vle angular es : ");
    Serial.println(motores_get_velocidad_angular_objetivo_temp());

    // mantengo constante
    motores_set_aceleracion_angular(0);
    Serial.print(encoders_get_posicion_left());
    while (encoders_get_posicion_total_left() * LONGITUD_PASO_ENCODER < parar_en)
    {
#ifdef MOTORES_LOG_PID
        log_print();
#endif
    }
    Serial.print("decelero en ");
    Serial.print(encoders_get_posicion_total_left() * LONGITUD_PASO_ENCODER);
    Serial.print(" - deberia en ");
    Serial.println(parar_en, 8);

    // decelero
    bool aux_bool = false;
    motores_set_aceleracion_angular(-motores_get_maxima_aceleracion_angular());
    while (motores_get_velocidad_angular_objetivo_temp() > 0) {
#ifdef MOTORES_LOG_PID
        log_print();
#endif
    }

    Serial.print("paro en: izq=");
    Serial.print(encoders_get_posicion_total_left() * LONGITUD_PASO_ENCODER, 5);
    Serial.print("paro en: der=");
    Serial.println(encoders_get_posicion_total_right() * LONGITUD_PASO_ENCODER, 5);
    motores_set_aceleracion_angular(0);
    encoders_reset_posicion_total();
    for (uint8_t aux = 0; aux<100; aux++) {
        Serial.print(encoders_get_posicion_total_left());
        Serial.print(" ");
    }
    */

    float accel = motores_get_maxima_aceleracion_lineal()/2;
    float maxvel = 2*PI*0.09 / 2;
    float radio = 0.09;
    float maxvel_left = maxvel * (radio + DISTANCIA_ENTRE_RUEDAS/2) / radio;
    float accel_left = accel * (radio + DISTANCIA_ENTRE_RUEDAS/2) / radio;

    motores_set_aceleracion_lineal(accel);
    motores_set_radio(radio);

    parar_en = 2*PI*0.09 - (maxvel_left * maxvel_left / (accel_left * 2));
    Serial.print("parar en ");
    Serial.println(parar_en);
    while(1) {
        if (motores_get_velocidad_lineal_objetivo_temp() >= maxvel) {
            motores_set_aceleracion_lineal(0);
        }
        if (encoders_get_posicion_total_left() * LONGITUD_PASO_ENCODER > parar_en)
            motores_set_aceleracion_lineal(-accel);

        Serial.print(motores_get_velocidad_lineal_objetivo_temp() + (motores_get_velocidad_angular_objetivo_temp() * DISTANCIA_ENTRE_RUEDAS / 2  ));
        Serial.print(" ");
        Serial.print(motores_get_velocidad_angular_objetivo_temp());
        Serial.print(" ");
        Serial.print(encoders_get_posicion_total_left());
        Serial.print(" ");
        Serial.print(encoders_get_posicion_total_right());
        Serial.print(" ");
        Serial.print(encoders_get_posicion_total_left() * LONGITUD_PASO_ENCODER);
        Serial.print(" ");
        Serial.println();

        if (motores_get_velocidad_lineal_objetivo_temp() < 0)
            break;

    }
    motores_set_aceleracion_lineal(0);
    motores_set_velocidad(0,0);
    Serial.print("Fin!");


    
#ifdef MOTORES_LOG_PID
    for (uint8_t aux = 0; aux<100; aux++) {
        log_print();
    }
#endif

}
