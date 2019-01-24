#ifndef __MOTORES_H
#define __MOTORES_H

#include <encoders.h>

// H-bridge pins
#define MOTOR_LEFT_PWM 5 
#define MOTOR_LEFT_IN1 9
#define MOTOR_LEFT_IN2 4

#define MOTOR_RIGHT_PWM 6 
#define MOTOR_RIGHT_IN1 7
#define MOTOR_RIGHT_IN2 10

#define MAX_PWM 200

void motores_init(void);
void motores_set_pwm(int16_t left, int16_t right);
void motores_set_pwm_left(int16_t right);
void motores_set_pwm_right(int16_t left);
int16_t motores_get_pwm_left();
int16_t motores_get_pwm_right();
void motores_set_ticks(int32_t left, int32_t right);
uint32_t motores_get_ticks_left();
uint32_t motores_get_ticks_right();
float motores_get_velocidad_actual_left();
float motores_get_velocidad_lineal_objetivo();
float motores_get_velocidad_angular_objetivo();
void motores_actualiza_velocidad();
void motores_set_velocidad(float lineal, float angular);
void motores_actualiza_angulo();
double motores_get_angulo_actual();
double motores_get_angulo_actual_calculado();
void motores_set_angulo_actual(double angulo);

#endif /* ifndef MOTORES_H

 */
