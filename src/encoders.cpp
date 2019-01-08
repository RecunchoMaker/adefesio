#include <encoders.h>

volatile int16_t encoder_posicion_left = 0;
volatile int16_t encoder_posicion_right = 0;

volatile uint8_t ticks_sin_actualizar_left = 0;
volatile uint8_t ticks_sin_actualizar_right = 0;

volatile uint16_t tcnt1_left[2] = {0, 0};
volatile uint16_t tcnt1_right[2] = {0, 0};

volatile int32_t ticks_left;
volatile int32_t last_ticks_left;

volatile int32_t ticks_right;
volatile int32_t last_ticks_right;

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
   
int16_t encoders_get_posicion_left(void) {
    return encoder_posicion_left;
}

int16_t encoders_get_posicion_right(void) {
    return encoder_posicion_right;
}

void encoders_ISR_left(void) {
    tcnt1_left[0] = tcnt1_left[1];
    tcnt1_left[1] = TCNT1;

    if (digitalRead(ENCODER_LEFT_C2) == digitalRead(ENCODER_LEFT_C1))
    {
        encoder_posicion_left--;
    }
    
    else
    {
        encoder_posicion_left++;
    }
}

void encoders_ISR_right(void) {
    if (digitalRead(ENCODER_RIGHT_C2) == digitalRead(ENCODER_RIGHT_C1))
        encoder_posicion_right++;
    else
        encoder_posicion_right--;
}

void encoders_calcula_ticks_left() {

    last_ticks_left = ticks_left;

    if (encoder_posicion_left >=2) {
        // caso 1
        ticks_left = tcnt1_left[1] - tcnt1_left[0];
        if (ticks_left < 0)
            ticks_left += OCR1A;

        ticks_sin_actualizar_left = 0;

    } else if (encoder_posicion_left == 1) {

        // caso 2
        ticks_left = OCR1A * (ticks_sin_actualizar_left+1) +
            tcnt1_left[1] - tcnt1_left[0];
        ticks_sin_actualizar_left = 0;
    }
    else {

        // caso 3
        ticks_sin_actualizar_left++;
        ticks_left = ticks_sin_actualizar_left * OCR1A + (OCR1A - tcnt1_left[0]);
        if (ticks_left < last_ticks_left) {
            ticks_left = last_ticks_left;
        }
    }
}

void encoders_calcula_ticks_right() {

    last_ticks_right = ticks_right;

    if (encoder_posicion_right >=2) {
        // caso 1
        ticks_right = tcnt1_right[1] - tcnt1_right[0];
        if (ticks_right < 0)
            ticks_right += OCR1A;

        ticks_sin_actualizar_right = 0;

    } else if (encoder_posicion_right == 1) {

        // caso 2
        ticks_right = OCR1A * (ticks_sin_actualizar_right+1) +
            tcnt1_right[1] - tcnt1_right[0];
        ticks_sin_actualizar_right = 0;
    }
    else {

        // caso 3
        ticks_sin_actualizar_right++;
        ticks_right = ticks_sin_actualizar_right * OCR1A + (OCR1A - tcnt1_right[0]);
        if (ticks_right < last_ticks_right) {
            ticks_right = last_ticks_right;
        }
    }
}

int32_t encoders_get_ticks_left() {
    return ticks_left;
}

int32_t encoders_get_ticks_right() {
    return ticks_right;
}