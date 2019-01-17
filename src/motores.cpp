#include <Arduino.h>
#include <settings.h>
#include <motores.h>

#define KA (200 / 0.28)
#define KP_LINEAL -200.0
#define KI_LINEAL -0.5

volatile int16_t pwm_left;
volatile int16_t pwm_right;

volatile float velocidad_lineal_objetivo = 0;
volatile float velocidad_lineal_actual = 0;

volatile float error_lineal_left = 0;
volatile float error_lineal_right = 0;

volatile float error_acumulado_left = 0;
volatile float error_acumulado_right = 0;

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

float motores_get_velocidad_actual() {
    return velocidad_lineal_actual;
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

void motores_actualizar_velocidad() {

    error_lineal_left = encoders_get_ultima_velocidad_left() - velocidad_lineal_objetivo;
    error_lineal_right = encoders_get_ultima_velocidad_right() - velocidad_lineal_objetivo;

    error_acumulado_left += error_lineal_left;
    error_acumulado_right += error_lineal_right;

    pwm_left = KA * velocidad_lineal_objetivo;
    pwm_left += KP_LINEAL * error_lineal_left;
    pwm_left += KI_LINEAL * error_acumulado_left;

    pwm_right = KA * velocidad_lineal_objetivo;
    pwm_right += KP_LINEAL * error_lineal_right;
    pwm_right += KI_LINEAL * error_acumulado_right;

    motores_set_pwm(pwm_left, pwm_right);

}

void motores_set_velocidad(float velocidad_lineal, float velocidad_angular) {

    if (velocidad_lineal > MAX_VELOCIDAD)
        velocidad_lineal_objetivo = MAX_VELOCIDAD;
    else
        velocidad_lineal_objetivo = velocidad_lineal;

}
