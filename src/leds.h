#ifndef __LEDS_H
#define __LEDS_H

#define LED_FIZQ A0
#define LED_IZQ A2
#define LED_DER A1
#define LED_FDER A3
#define LED_SENSOR A4

void leds_init();
void leds_activa();
void leds_desactiva();
void leds_enciende(int8_t led);
void leds_apaga(int8_t led);
void leds_actualiza_valor(int8_t sensor);
int16_t leds_get_valor(int8_t sensor);

#endif
