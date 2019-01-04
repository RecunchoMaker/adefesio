#include <encoders.h>

volatile int16_t encoder_posicion_left = 0;
volatile int16_t encoder_posicion_right = 0;

volatile uint8_t ticks_sin_actualizar_left = 0;

volatile uint16_t tcnt1_left[2] = {0, 0};
volatile uint16_t tcnt1_right[2] = {0, 0};

volatile uint32_t encoder_ticks;

volatile uint32_t last_encoder_ticks;
volatile int16_t  last_position_left;
volatile int8_t   last_ticks_sin_actualizar_left;
volatile uint16_t last_tcnt1_left[2] = {0, 0};
volatile uint8_t  caso = 0;

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
            encoders_ISR_right, FALLING);
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
    /*
    if (tcnt1_left[0] == 0)
        tcnt1_left[0] = TCNT1;
    else
        tcnt1_left[1] = TCNT1;
        */
    tcnt1_left[0] = tcnt1_left[1];
    tcnt1_left[1] = TCNT1;

    last_tcnt1_left[0] = tcnt1_left[0];
    last_tcnt1_left[1] = tcnt1_left[1];

    if (digitalRead(ENCODER_LEFT_C2) == digitalRead(ENCODER_LEFT_C1))
    {
        //Serial.print("ein?");
        encoder_posicion_left--;
    }
    
    else
    {
        //Serial.print("vale");
        encoder_posicion_left++;
    }
}

void encoders_ISR_right(void) {
    if (digitalRead(ENCODER_RIGHT_C2) == digitalRead(ENCODER_RIGHT_C1))
        encoder_posicion_right++;
    else
        encoder_posicion_right--;
}

void encoders_print(void) {
        Serial.print("caso:");
        Serial.print(caso);
        Serial.print("   sin act:");
        Serial.print(last_ticks_sin_actualizar_left);
        Serial.print("   encoder left: ");
        Serial.print(last_position_left);
        Serial.print("   ticks: ");
        Serial.print(last_encoder_ticks);
        Serial.print("   0,1 = ");
        Serial.print(last_tcnt1_left[0]);
        Serial.print(" ");
        Serial.print(last_tcnt1_left[1]);
        Serial.print(" ");
        Serial.println();
}

uint32_t encoders_get_ticks_entre_saltos_left() {


    last_position_left = encoder_posicion_left;;
    if (encoder_posicion_left >= 2) {

        // caso 1
        caso = 1;
        encoder_ticks = tcnt1_left[1] - tcnt1_left[0];
        ticks_sin_actualizar_left = 0;
        last_ticks_sin_actualizar_left = ticks_sin_actualizar_left;
        last_encoder_ticks = encoder_ticks;
        return encoder_ticks ;

    } else if (encoder_posicion_left == 1) {

        // caso 2
        caso = 2;
        encoder_ticks = OCR1A * (ticks_sin_actualizar_left+1) +
            tcnt1_left[1] - tcnt1_left[0];

        if (tcnt1_left[0] > 17985)
        {
        Serial.println("caso 2");
        encoders_print();
        }
        ticks_sin_actualizar_left = 0;
        last_ticks_sin_actualizar_left = ticks_sin_actualizar_left;
        last_encoder_ticks = encoder_ticks;
        return  encoder_ticks;
    }
    else {

        // caso 3
        caso = 3;
        ticks_sin_actualizar_left++;
        last_ticks_sin_actualizar_left = ticks_sin_actualizar_left;
        last_encoder_ticks = ticks_sin_actualizar_left * OCR1A;
        return ticks_sin_actualizar_left * OCR1A;
    }
}

        

