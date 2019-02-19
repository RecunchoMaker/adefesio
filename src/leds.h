#ifndef __LEDS_H
#define __LEDS_H

#define LED_IZQ A1
#define LED_DER A2
#define LED_FIZQ A3
#define LED_FDER A4
#define LED_SENSOR A5

void leds_init();
void leds_activa();
void leds_desactiva();
void leds_enciende(int8_t led);
void leds_apaga(int8_t led);
void leds_actualiza_valor(int8_t sensor);
int16_t leds_get_valor(int8_t sensor);

#endif
