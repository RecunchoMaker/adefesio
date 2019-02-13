#include <encoders.h>
#include <motores.h>
#include <settings.h>
#include <timer1.h>

#define DIRECCION_ADELANTE 1
#define DIRECCION_ATRAS 0
#define MIN_TCNT1 200


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

volatile float velocidad_left = 0;
volatile float velocidad_right = 0;

volatile float ultima_velocidad_left = 0;
volatile float ultima_velocidad_right = 0;

volatile bool direccion_left = DIRECCION_ADELANTE;
volatile bool direccion_right = DIRECCION_ATRAS;

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

uint16_t encoders_get_tcnt1_anterior_right() {
    return tcnt1_anterior_right;
}

uint16_t encoders_get_ultimo_tcnt1_right() {
    return ultimo_tcnt1_right;
}

void encoders_set_direccion(bool left, bool right) {
    direccion_left = left;
    direccion_right = right;
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

int32_t encoders_get_posicion_total(void) {
    return (encoder_posicion_total_right + encoder_posicion_total_left) / 2;
}

void encoders_ISR_left(void) {

    ultimo_tcnt1_left = OCR1A * timer1_get_estado() + TCNT1;
    /*
    if (ultimo_tcnt1_left < MIN_TCNT1)
        ultimo_tcnt1_left = OCR1A * NUMERO_ESTADOS;
        */

    if (direccion_left)
    {
        encoder_posicion_left++;
        encoder_posicion_total_left++;
    }
    
    else
    {
        encoder_posicion_left--;
        encoder_posicion_total_left--;
    }
}

void encoders_ISR_right(void) {

    ultimo_tcnt1_right = OCR1A * timer1_get_estado() + TCNT1;
    /*
    if (ultimo_tcnt1_right < MIN_TCNT1)
        ultimo_tcnt1_right = OCR1A * NUMERO_ESTADOS;
        */

    if (direccion_right)
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
        velocidad_left = ticks_sin_actualizar_left < 10 ? ultima_velocidad_left : 0;
    }
    else {

        velocidad_left = LONGITUD_PASO_ENCODER * encoder_posicion_left * OCR1A_POR_CICLO /
            (PERIODO_CICLO * ( (int32_t) OCR1A_POR_CICLO * (ticks_sin_actualizar_left + 1) + ultimo_tcnt1_left - tcnt1_anterior_left));

        if ((velocidad_left < 0 and direccion_left == DIRECCION_ADELANTE)
            or
            (velocidad_left > 0 and direccion_left == DIRECCION_ATRAS))
        {
            ticks_sin_actualizar_left++;
            velocidad_left = LONGITUD_PASO_ENCODER * encoder_posicion_left * OCR1A_POR_CICLO /
                (PERIODO_CICLO * ( (int32_t) OCR1A_POR_CICLO * (ticks_sin_actualizar_left + 1) + ultimo_tcnt1_left - tcnt1_anterior_left));
        }

        ticks_sin_actualizar_left = 0;
        tcnt1_anterior_left = ultimo_tcnt1_left;
    }

    ultima_velocidad_left = velocidad_left;

    if (encoder_posicion_right == 0) {
        ticks_sin_actualizar_right++;
        velocidad_right = ticks_sin_actualizar_right < 10 ? ultima_velocidad_right : 0;
    }
    else {
        velocidad_right = LONGITUD_PASO_ENCODER * encoder_posicion_right * OCR1A_POR_CICLO /
            (PERIODO_CICLO * ( (int32_t) OCR1A_POR_CICLO * (ticks_sin_actualizar_right + 1) + ultimo_tcnt1_right - tcnt1_anterior_right));

        if ((velocidad_right < 0 and direccion_right == DIRECCION_ADELANTE)
            or
            (velocidad_right > 0 and direccion_right == DIRECCION_ATRAS))
        {
            ticks_sin_actualizar_right++;
            velocidad_right = LONGITUD_PASO_ENCODER * encoder_posicion_right * OCR1A_POR_CICLO /
                (PERIODO_CICLO * ( (int32_t) OCR1A_POR_CICLO * (ticks_sin_actualizar_right + 1) + ultimo_tcnt1_right - tcnt1_anterior_right));
        }

        ticks_sin_actualizar_right = 0;
        tcnt1_anterior_right = ultimo_tcnt1_right;
    }

    ultima_velocidad_right = velocidad_right;
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

