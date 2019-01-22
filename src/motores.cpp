#include <Arduino.h>
#include <settings.h>
#include <motores.h>

#define KA (200 / 0.28)
#define KP_LINEAL -300.0
#define KI_LINEAL -1.0

volatile int16_t pwm_left;
volatile int16_t pwm_right;

volatile float velocidad_lineal_objetivo = 0;
volatile float velocidad_lineal_actual_left = 0;
volatile float velocidad_lineal_actual_right = 0;

volatile float velocidad_angular_objetivo = 0;
volatile double angulo_actual = 0;
volatile double angulo_actual_calculado = 0;

volatile float error_lineal_left = 0;
volatile float error_lineal_right = 0;

volatile float error_acumulado_left = 0;
volatile float error_acumulado_right = 0;

volatile double aux_e1 = 0;
volatile double aux_e2 = 0;

void motores_init(void) {

    pinMode(MOTOR_LEFT_IN1, OUTPUT);
    pinMode(MOTOR_LEFT_IN2, OUTPUT);
    pinMode(MOTOR_LEFT_PWM, OUTPUT);
    pinMode(MOTOR_RIGHT_IN1, OUTPUT);
    pinMode(MOTOR_RIGHT_IN2, OUTPUT);
    pinMode(MOTOR_RIGHT_PWM, OUTPUT);

    motores_set_pwm(0,0);
}

void motores_set_pwm(int16_t left, int16_t right) {

    if (right > 0) {
        if (right > MAX_PWM) right = MAX_PWM;
        digitalWrite(MOTOR_RIGHT_IN1, HIGH);
        digitalWrite(MOTOR_RIGHT_IN2, LOW);
        analogWrite(MOTOR_RIGHT_PWM, right);
        pwm_right = right;
    } else {
        if (right < -MAX_PWM) right = -MAX_PWM;
        digitalWrite(MOTOR_RIGHT_IN1, LOW);
        digitalWrite(MOTOR_RIGHT_IN2, HIGH);
        analogWrite(MOTOR_RIGHT_PWM, -right);
        pwm_right = -right;
    }

    if (left > 0) {
        if (left > MAX_PWM) left = MAX_PWM;
        digitalWrite(MOTOR_LEFT_IN1, HIGH);
        digitalWrite(MOTOR_LEFT_IN2, LOW);
        analogWrite(MOTOR_LEFT_PWM, left);
        pwm_left = left;
    } else {
        if (left < -MAX_PWM) left = -MAX_PWM;
        digitalWrite(MOTOR_LEFT_IN1, LOW);
        digitalWrite(MOTOR_LEFT_IN2, HIGH);
        analogWrite(MOTOR_LEFT_PWM, -left);
        pwm_left = -left;
    }


}

float motores_get_velocidad_actual_left() {
    return velocidad_lineal_actual_left;
}

int16_t motores_get_pwm_left() {
    return pwm_left;
}

int16_t motores_get_pwm_right() {
    return pwm_right;
}

float motores_get_velocidad_lineal_objetivo() {
    return velocidad_lineal_objetivo;
}

float motores_get_velocidad_angular_objetivo() {
    return velocidad_angular_objetivo;
}

void motores_actualiza_velocidad() {

    if (velocidad_angular_objetivo != 0 or velocidad_lineal_objetivo != 0) {

        error_lineal_left = encoders_get_ultima_velocidad_left() - 
           (velocidad_lineal_objetivo + (velocidad_angular_objetivo * DISTANCIA_ENTRE_RUEDAS / 2  ));
        error_lineal_right = encoders_get_ultima_velocidad_right() -
           (velocidad_lineal_objetivo - (velocidad_angular_objetivo * DISTANCIA_ENTRE_RUEDAS / 2  ));

        error_acumulado_left += error_lineal_left;
        error_acumulado_right += error_lineal_right;

        pwm_left = KA * (velocidad_lineal_objetivo + (velocidad_angular_objetivo * DISTANCIA_ENTRE_RUEDAS / 2));
        pwm_left += KP_LINEAL * error_lineal_left; 
        pwm_left += KI_LINEAL * error_acumulado_left;

        pwm_right = KA * (velocidad_lineal_objetivo - (velocidad_angular_objetivo * DISTANCIA_ENTRE_RUEDAS / 2));
        pwm_right += KP_LINEAL * error_lineal_right;
        pwm_right += KI_LINEAL * error_acumulado_right;

        motores_set_pwm(pwm_left, pwm_right);
    }
    else {
        motores_set_pwm(0, 0);
    }
}

void motores_set_velocidad(float velocidad_lineal, float velocidad_angular) {

    if (velocidad_lineal > MAX_VELOCIDAD)
        velocidad_lineal_objetivo = MAX_VELOCIDAD;
    else
        velocidad_lineal_objetivo = velocidad_lineal;

    velocidad_angular_objetivo = velocidad_angular;

}

double motores_get_angulo_actual() {
    return angulo_actual;
}

void motores_set_angulo_actual(double angulo) {
    angulo_actual = angulo;
}

double motores_get_angulo_actual_calculado() {
    return angulo_actual_calculado;
}

void motores_actualiza_angulo() {

    /*
     * Forma 1 de calcular el angulo, presuponiendo que actualiza_velicidad()
     * funciona perfectamente
     *
    angulo_actual_calculado += velocidad_angular_objetivo * PERIODO_TIMER;
    */

    /*
     * Forma 2 de calcular el angulo
     *
    aux_e1 = encoders_get_ultima_velocidad_left() * PERIODO_TIMER;
    aux_e2 = encoders_get_ultima_velocidad_right() * PERIODO_TIMER;
        angulo_actual_calculado += (aux_e1 - aux_e2) / DISTANCIA_ENTRE_RUEDAS;
    */

    if (encoders_get_posicion_left() != encoders_get_posicion_right) {
        angulo_actual+= (LONGITUD_PASO_ENCODER * encoders_get_posicion_left() -
                         LONGITUD_PASO_ENCODER * encoders_get_posicion_right())
                         / 
                         DISTANCIA_ENTRE_RUEDAS;
        if (angulo_actual < 0) angulo_actual+=2*PI;
        else if (angulo_actual>2*PI) angulo_actual -= 2*PI;
    }

}
