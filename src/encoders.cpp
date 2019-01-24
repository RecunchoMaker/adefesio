#include <encoders.h>
#include <motores.h>
#include <settings.h>

#define KALMAN_GAIN 1

volatile int16_t encoder_posicion_left = 0;
volatile int16_t encoder_posicion_right = 0;

volatile int32_t encoder_posicion_total_left = 0;
volatile int32_t encoder_posicion_total_right = 0;

volatile uint8_t ticks_sin_actualizar_left = 0;
volatile uint8_t ticks_sin_actualizar_right = 0;

volatile int32_t tcnt1_anterior_left = 0;
volatile int32_t ultimo_tcnt1_left = 0;

volatile uint16_t tcnt1_anterior_right = 0;
volatile uint16_t ultimo_tcnt1_right = 0;
volatile uint16_t michi = 0;

volatile double velocidad_left = 0;
volatile double velocidad_right = 0;

volatile double ultima_velocidad_left = 0;
volatile double ultima_velocidad_right = 0;

void encoders_init(void) {

    // pin modes
    pinMode(ENCODER_LEFT_C1, INPUT);
    pinMode(ENCODER_LEFT_C2, INPUT);
    pinMode(ENCODER_RIGHT_C1, INPUT);
    pinMode(ENCODER_RIGHT_C2, INPUT);

    // inicialir interrupciones
    attachInterrupt(
            digitalPinToInterrupt(ENCODER_LEFT_C1),
            encoders_ISR_left, RISING);
    attachInterrupt(
            digitalPinToInterrupt(ENCODER_RIGHT_C1),
            encoders_ISR_right, RISING);
}

void encoders_reset_posicion(void) {
    encoder_posicion_left = 0;
    encoder_posicion_right = 0;
}
   
void encoders_reset_posicion_total(void) {
    encoder_posicion_total_left = 0;
    encoder_posicion_total_right = 0;
}

uint8_t encoders_get_ticks_sin_actualizar_left(void) {
    return ticks_sin_actualizar_left;
}

uint8_t encoders_get_ticks_sin_actualizar_right(void) {
    return ticks_sin_actualizar_right;
}

int16_t encoders_get_posicion_left(void) {
    return encoder_posicion_left;
}

int16_t encoders_get_posicion_right(void) {
    return encoder_posicion_right;
}

int32_t encoders_get_posicion_total_left(void) {
    return encoder_posicion_total_left;
}

int32_t encoders_get_posicion_total_right(void) {
    return encoder_posicion_total_right;
}

void encoders_ISR_left(void) {

    ultimo_tcnt1_left = TCNT1;

    if (digitalRead(ENCODER_LEFT_C2) == digitalRead(ENCODER_LEFT_C1))
    {
        encoder_posicion_left--;
        encoder_posicion_total_left--;
    }
    
    else
    {
        encoder_posicion_left++;
        encoder_posicion_total_left++;
    }
}

void encoders_ISR_right(void) {

    ultimo_tcnt1_right = TCNT1;

    if (digitalRead(ENCODER_RIGHT_C2) == digitalRead(ENCODER_RIGHT_C1))
    {
        encoder_posicion_right++;
        encoder_posicion_total_right++;
    }
    else {
        encoder_posicion_right--;
        encoder_posicion_total_right--;
    }
}

void encoders_calcula_velocidad() {

    if (encoder_posicion_left == 0) {
        ticks_sin_actualizar_left++;
        velocidad_left = ultima_velocidad_left;
    }
    else {

        velocidad_left = LONGITUD_PASO_ENCODER * encoder_posicion_left * OCR1A/
            (PERIODO_TIMER * ( (int32_t) OCR1A * (ticks_sin_actualizar_left + 1) + ultimo_tcnt1_left - tcnt1_anterior_left));

        velocidad_left = KALMAN_GAIN * velocidad_left + (1-KALMAN_GAIN) * ultima_velocidad_left;
        ticks_sin_actualizar_left = 0;
        tcnt1_anterior_left = ultimo_tcnt1_left;
        ultima_velocidad_left = velocidad_left;
        // encoder_posicion_left = 0;

    }

    if (encoder_posicion_right == 0) {
        ticks_sin_actualizar_right++;
        velocidad_right = ultima_velocidad_right;
    }
    else {
        velocidad_right = LONGITUD_PASO_ENCODER * encoder_posicion_right * OCR1A /
            (PERIODO_TIMER * ( (int32_t) OCR1A * (ticks_sin_actualizar_right + 1) + ultimo_tcnt1_right - tcnt1_anterior_right));

        velocidad_right = KALMAN_GAIN * velocidad_right + (1-KALMAN_GAIN) * ultima_velocidad_right;
        ticks_sin_actualizar_right = 0;
        tcnt1_anterior_right = ultimo_tcnt1_right;
        ultima_velocidad_right = velocidad_right;
        // encoder_posicion_right = 0;
    }
}

float encoders_get_velocidad_left() {
    return velocidad_left;
}

float encoders_get_velocidad_right() {
    return velocidad_right;
}

float encoders_get_ultima_velocidad_left() {
    return ultima_velocidad_left;
}

float encoders_get_ultima_velocidad_right() {
    return ultima_velocidad_right;
}

