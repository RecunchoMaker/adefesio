#ifndef __ENCODERS_H
#define __ENCODERS_H

#include <Arduino.h>

#define ENCODER_LEFT_C1 2
#define ENCODER_LEFT_C2 12
#define ENCODER_RIGHT_C1 3
#define ENCODER_RIGHT_C2 8

void encoders_init(void);
void encoders_reset_posicion(void);
int16_t encoders_get_posicion_left(void);
int16_t encoders_get_posicion_right(void);
void encoders_ISR_left(void);
void encoders_ISR_right(void);
float encoders_get_velocidad_left(void);
float encoders_get_velocidad_right(void);
void encoders_calcula_ticks_left();
void encoders_calcula_ticks_right();
int32_t encoders_get_ticks_left();
int32_t encoders_get_ticks_right();

#endif /* ifndef _ENCODERS_H

 */
