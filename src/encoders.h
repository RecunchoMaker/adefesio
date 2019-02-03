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
int16_t encoders_get_posicion_left(void);
int16_t encoders_get_posicion_right(void);
int32_t encoders_get_posicion_total_left(void);
int32_t encoders_get_posicion_total_right(void);
int32_t encoders_get_posicion_total(void);
void encoders_ISR_left(void);
void encoders_ISR_right(void);
float encoders_get_velocidad_left(void);
float encoders_get_velocidad_right(void);
float encoders_get_ultima_velocidad_left(void);
float encoders_get_ultima_velocidad_right(void);
uint8_t encoders_get_ticks_sin_actualizar_left(void);
uint8_t encoders_get_ticks_sin_actualizar_right(void);
void encoders_calcula_velocidad();
void encoders_set_direccion(uint8_t left, uint8_t right);

#ifdef ENCODERS_LOG_ESTADO
void encoders_log_estado_cabecera();
void encoders_log_estado();
#endif

#endif /* ifndef _ENCODERS_H

 */
