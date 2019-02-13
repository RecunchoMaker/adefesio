#ifndef __TIMER1_H
#define __TIMER1_H

#include <Arduino.h>

void timer1_init(float period, uint16_t prescaler);
void timer1_incrementa_cuenta();
void timer1_reset_cuenta();
uint32_t timer1_get_cuenta();
int8_t timer1_get_estado();

#endif /* ifndef TIMER1_H

 */

