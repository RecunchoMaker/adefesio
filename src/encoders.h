/**
 * @file encoders.h
 *
 * @brief Cabecera de encoders.cpp
 */

#ifndef __ENCODERS_H
#define __ENCODERS_H

#include <Arduino.h>

#define ENCODER_LEFT_C1 2
#define ENCODER_LEFT_C2 12
#define ENCODER_RIGHT_C1 3
#define ENCODER_RIGHT_C2 8

void encoders_init(void);

void encoders_reset_posicion(void);
void encoders_reset_posicion_total(void);
void encoders_reset_posicion_aux_total(void);
void encoders_decrementa_posicion_total(int16_t pasos);

int32_t encoders_get_posicion_total_left(void);
int32_t encoders_get_posicion_total_right(void);
int32_t encoders_get_posicion_aux_left(void);
int32_t encoders_get_posicion_aux_right(void);
int32_t encoders_get_posicion_aux(void);

uint8_t encoders_get_ticks_sin_actualizar_left(void);
uint8_t encoders_get_ticks_sin_actualizar_right(void);
uint16_t encoders_get_tcnt1_anterior_right();
uint16_t encoders_get_ultimo_tcnt1_right();

float encoders_get_velocidad_left(void);
float encoders_get_velocidad_right(void);
float encoders_get_ultima_velocidad_left(void);
float encoders_get_ultima_velocidad_right(void);

int32_t encoders_get_posicion_total(void);
void encoders_set_direccion(bool left, bool right);

void encoders_ISR_left(void);
void encoders_ISR_right(void);

void encoders_calcula_velocidad();

#endif /* ifndef _ENCODERS_H

 */
