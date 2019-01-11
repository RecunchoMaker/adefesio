#include <encoders.h>
#include <motores.h>
#include <settings.h>

volatile int16_t encoder_posicion_left = 0;
volatile int16_t encoder_posicion_right = 0;

volatile int32_t encoder_posicion_left_total = 0;
volatile int32_t encoder_posicion_right_total = 0;

volatile uint8_t ticks_sin_actualizar_left = 0;
volatile uint8_t ticks_sin_actualizar_right = 0;

volatile uint16_t tcnt1_left[2] = {0, 0};
volatile uint16_t tcnt1_right[2] = {0, 0};

volatile int32_t ticks_left;
volatile int32_t last_ticks_left;

volatile int32_t ticks_right;
volatile int32_t last_ticks_right;

volatile float radio = 999999;
volatile float velocidad_angular = 0;

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
        encoder_posicion_left_total--;
    }
    
    else
    {
        encoder_posicion_left++;
        encoder_posicion_left_total++;
    }
}

void encoders_ISR_right(void) {
    tcnt1_right[0] = tcnt1_right[1];
    tcnt1_right[1] = TCNT1;

    if (digitalRead(ENCODER_RIGHT_C2) == digitalRead(ENCODER_RIGHT_C1))
    {
        encoder_posicion_right++;
        encoder_posicion_right_total++;
    }
    else {
        encoder_posicion_right--;
        encoder_posicion_right_total--;
    }
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

void encoders_calcula_velocidad_angular() {
    
    if (ticks_left == ticks_right) {
        radio = 99999;
        velocidad_angular = 0;
        //Serial.print("%");
        return 0;
    }
    if ((ticks_left == 0) or (ticks_right == 0)) {
        radio = 99999;
        velocidad_angular = 0;
        return 0;
    }

    if (ticks_left < ticks_right) {

        float dif = OCR1A / ticks_left - OCR1A / ticks_right;
        if (dif == 0) return 0;

        radio = (1.0*OCR1A/ticks_left + 1.0*OCR1A/ticks_right) * DISTANCIA_ENTRE_EJES /
                     (dif);
        if (radio < 0) Serial.print("R");
        velocidad_angular = (OCR1A * 1.0 /ticks_left) /
            (2 * PI * (radio - DISTANCIA_ENTRE_EJES));
    }
    else {
        float dif = (1.0 * OCR1A / ticks_right) - (1.0 * OCR1A / ticks_left);
        if (dif == 0) return 0;

        radio = (1.0 * OCR1A/ticks_left + 1*0 * OCR1A/ticks_right) * DISTANCIA_ENTRE_EJES /
                     (dif);
        velocidad_angular = -(OCR1A * 1.0 /ticks_right) /
            (2 * PI * (radio - DISTANCIA_ENTRE_EJES));
    }
}


int32_t encoders_get_ticks_left() {
    return ticks_left;
}

int32_t encoders_get_ticks_right() {
    return ticks_right;
}

float encoders_get_radio() {
    return radio;
}

float encoders_get_velocidad_angular() {
    return velocidad_angular;
}

#ifdef ENCODERS_LOG_ESTADO
void encoders_log_estado_cabecera() {
    Serial.println("deseados obtenidos pwmLeft pwmRight posLeft posRight ticksSinAct radio velocidadAngular");
}

void encoders_log_estado() {
    LOG(motores_get_ticks_right());
    LOG(encoders_get_ticks_right());
    LOG(motores_get_pwm_left());
    LOG(motores_get_pwm_right());
    LOG(encoder_posicion_left_total);
    LOG(encoder_posicion_right_total);
    LOG(ticks_sin_actualizar_right);
    LOG(radio);
    LOGN(velocidad_angular);

}
#endif
