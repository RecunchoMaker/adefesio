/**
 * @file timer1.h
 *
 * @brief Cabecera de timer1.cpp
 */

#ifndef __TIMER1_H
#define __TIMER1_H

#include <Arduino.h>

void timer1_init(float period, uint16_t prescaler);
uint32_t timer1_get_cuenta();
int8_t timer1_get_estado();
void timer1_reset_cuenta();
void timer1_espera(uint16_t ciclos);
void timer1_incrementa_cuenta();

#endif /* ifndef TIMER1_H

 */

