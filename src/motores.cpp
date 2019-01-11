#include <Arduino.h>
#include <motores.h>

#define KP_LINEAL 0.00029
#define KP_ANGULAR -0.250

volatile int16_t pwm_left;
volatile int16_t pwm_right;

volatile int32_t ticks_deseados_left;
volatile int32_t ticks_deseados_right;

volatile int32_t error_lineal_left = 0;
volatile int32_t error_lineal_right = 0;

volatile float error_angular_anterior = 0;

void motores_init(void) {

    pinMode(MOTOR_LEFT_IN1, OUTPUT);
    pinMode(MOTOR_LEFT_IN2, OUTPUT);
    pinMode(MOTOR_LEFT_PWM, OUTPUT);
    pinMode(MOTOR_RIGHT_IN1, OUTPUT);
    pinMode(MOTOR_RIGHT_IN2, OUTPUT);
    pinMode(MOTOR_RIGHT_PWM, OUTPUT);

    motores_set_pwm(0,0);
    motores_set_ticks(999999,999999);
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

    error_lineal_left = ticks_deseados_left - encoders_get_ticks_left();
    error_lineal_right = ticks_deseados_right - encoders_get_ticks_right();

    // TODO: Velocidad angular deseada, siempre 0
    float error_angular = encoders_get_velocidad_angular() * KP_ANGULAR;

    motores_set_pwm(
            (pwm_left - error_lineal_left * KP_LINEAL) +
            error_angular - error_angular_anterior
            ,
            (pwm_right - error_lineal_right * KP_LINEAL) -
            error_angular + error_angular_anterior
            );
    error_angular_anterior = error_angular;
}

    

