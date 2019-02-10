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

void motores_init(float voltaje);

void motores_set_kp_lineal(float kp);
void motores_set_ki_lineal(float ki);
void motores_set_kd_lineal(float kd);
void motores_set_kp_angular(float kd);
void motores_set_distancia_entre_ruedas(float dr);
float motores_get_kp_lineal();
float motores_get_kp_angular();
float motores_get_ki_lineal();
float motores_get_kd_lineal();
float motores_get_distancia_entre_ruedas();

void motores_set_aceleracion_lineal(float aceleracion);
float motores_get_aceleracion_lineal();
void motores_set_radio(float r);
void motores_parar();

void motores_set_maximo_pwm(int16_t pwm);
void motores_set_pwm_left(int16_t right);
void motores_set_pwm_right(int16_t left);
void motores_set_potencia(float left, float right);
int16_t motores_get_pwm_left();
int16_t motores_get_pwm_right();
void motores_set_ticks(int32_t left, int32_t right);
uint32_t motores_get_ticks_left();
uint32_t motores_get_ticks_right();
void motores_set_velocidad_lineal_objetivo(float velocidad);
float motores_get_velocidad_lineal_objetivo();
float motores_get_velocidad_lineal_objetivo_left();
float motores_get_velocidad_lineal_objetivo_right();
void motores_actualiza_velocidad();
void motores_actualiza_angulo();
double motores_get_angulo_actual();
double motores_get_angulo_actual_calculado();
void motores_set_angulo_actual(double angulo);

#endif /* ifndef MOTORES_H

 */
