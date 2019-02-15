#ifndef __BATERIA_H
#define __BATERIA_H

#include <Arduino.h>

#define BATERIA_VOLTAJE_UMBRAL 7.2
#define BATERIA_VOLTAJE_USB 4.90
#define BATERIA_VOLTAJE_PIN A5
#define BATERIA_LED_PIN 13

void bateria_init(void);
float bateria_get_voltaje(void);
bool bateria_agotada(void);
void bateria_muestra_nivel(void);
void bateria_watchdog(void);

#endif /* ifndef BATERIA_H

 */
