#include <Arduino.h>
#include <motores.h>

//#define KP 0.00035
#define KP 0.00035
#define KD 0.000

int16_t pwm_left;
int16_t pwm_right;

int32_t ticks_deseados_left;
int32_t ticks_deseados_right;

int32_t error_left = 0;
int32_t error_right = 0;
int16_t error_acumulado_left = 0;
int16_t error_acumulado_right = 0;

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

int16_t motores_get_pwm_left() {
    return pwm_left;
}

int16_t motores_get_pwm_right() {
    return pwm_right;
}

void motores_set_ticks(int32_t left, int32_t right) {
    ticks_deseados_left = left;
    ticks_deseados_right = right;
}

int32_t motores_get_ticks_left() {
    return ticks_deseados_left;
}

int32_t motores_get_ticks_right() {
    return ticks_deseados_right;
}

void motores_actualizar_pwm() {

    error_left = ticks_deseados_left - encoders_get_ticks_left();
    error_right = ticks_deseados_right - encoders_get_ticks_right();

    motores_set_pwm(pwm_left - error_left * KP, pwm_right - error_right * KP);
}

    

