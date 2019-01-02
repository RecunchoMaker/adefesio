#include <encoders.h>

volatile int16_t encoder_posicion_left = 0;
volatile int16_t encoder_posicion_right = 0;

void encoders_init(void) {

    // pin modes
    pinMode(ENCODER_LEFT_C1, INPUT);
    pinMode(ENCODER_LEFT_C2, INPUT);
    pinMode(ENCODER_RIGHT_C1, INPUT);
    pinMode(ENCODER_RIGHT_C2, INPUT);

    // inicialir interrupciones
    attachInterrupt(
            digitalPinToInterrupt(ENCODER_LEFT_C1),
            encoders_ISR_left, CHANGE);
    attachInterrupt(
            digitalPinToInterrupt(ENCODER_RIGHT_C1),
            encoders_ISR_right, CHANGE);
}

void encoders_reset_posicion(void) {
    encoder_posicion_left = 0;
    encoder_posicion_right = 0;
}
   
int16_t encoders_get_posicion_left(void) {
    return encoder_posicion_left;
}

int16_t encoders_get_posicion_right(void) {
    return encoder_posicion_right;
}

void encoders_ISR_left(void) {
    if (digitalRead(ENCODER_LEFT_C2) == digitalRead(ENCODER_LEFT_C1))
        encoder_posicion_left--;
    else
        encoder_posicion_left++;
}

void encoders_ISR_right(void) {
    if (digitalRead(ENCODER_RIGHT_C2) == digitalRead(ENCODER_RIGHT_C1))
        encoder_posicion_right++;
    else
        encoder_posicion_right--;
}
