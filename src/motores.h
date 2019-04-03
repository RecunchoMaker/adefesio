/**
 * @file motores.h
 *
 * @brief Cabecera de motores.cpp
 */

#ifndef __MOTORES_H
#define __MOTORES_H

#include <encoders.h>

//@{
/**
 * @name Pins del puente H
 */

#define MOTOR_LEFT_PWM 5   ///< Pin digital de salida PWM izquierdo 
#define MOTOR_LEFT_IN1 4   ///< Pin digital de salida IN1 izquierdo 
#define MOTOR_LEFT_IN2 9   ///< Pin digital de salida IN1 izquierdo 

#define MOTOR_RIGHT_PWM 6  ///< Pin digital de salida PWM izquierdo 
#define MOTOR_RIGHT_IN1 7  ///< Pin digital de salida IN1 derecho
#define MOTOR_RIGHT_IN2 10 ///< Pin digital de salida IN2 derecho
//@}

// TODO: fix this
#define GIRO_IZQUIERDA_TODO 0
#define GIRO_DERECHA_TODO   0.01

void motores_init(float voltaje);
void motores_set_potencia(float left, float right);
void motores_set_pwm_left(int16_t right);
void motores_set_pwm_right(int16_t left);
void motores_parar();
void motores_actualiza_velocidad();

float motores_get_kp_lineal();
void motores_set_kp_lineal(float kp);
float motores_get_ki_lineal();
void motores_set_ki_lineal(float ki);
float motores_get_kd_lineal();
void motores_set_kd_lineal(float kd);
float motores_get_kp_pasillo1();
void motores_set_kp_pasillo1(float kd);
float motores_get_kp_pasillo2();
void motores_set_kp_pasillo2(float kd);

float motores_get_velocidad_lineal_objetivo();
float motores_get_velocidad_lineal_objetivo_left();
float motores_get_velocidad_lineal_objetivo_right();
float motores_get_radio();
float motores_get_radio_aux();
void motores_set_velocidad_lineal_objetivo(float velocidad);

int16_t motores_get_pwm_left();
int16_t motores_get_pwm_right();

void motores_set_maximo_pwm(int16_t pwm);
float motores_get_velocidad_angular_objetivo();
float motores_get_distancia_entre_ruedas();
void motores_set_distancia_entre_ruedas(float dr);

float motores_get_aceleracion_lineal();
void motores_set_aceleracion_lineal(float aceleracion);

void motores_set_radio(float r);

#endif /* ifndef MOTORES_H

 */
