/**
 * @file encoders.cpp
 *
 * @brief Funciones para controlar los encoders del motor
 */

#include <encoders.h>
#include <motores.h>
#include <settings.h>
#include <timer1.h>

enum tipo_direccion {ENCODER_ATRAS, ENCODER_ADELANTE};

//@{
/** @name Variables contadoras de saltos de encoder
 *
 * Se utilizan varios contadores para cada encoder. Uno funciona como
 * _contador_parcial_, para cálculos internos en este mismo fichero,
 * y el los demás son accesibles con un getter público para cálculos de más
 * alto nivel
 */

volatile int16_t encoder_posicion_left = 0;
volatile int16_t encoder_posicion_right = 0;

volatile int32_t encoder_posicion_total_left = 0;
volatile int32_t encoder_posicion_total_right = 0;

volatile int32_t encoder_posicion_aux_left = 0;
volatile int32_t encoder_posicion_aux_right = 0;
//@}


//@{
/** @name Acumulan los ciclos de reloj en los que no hay información nueva de los encoders
 */
volatile uint8_t ticks_sin_actualizar_left = 0;
volatile uint8_t ticks_sin_actualizar_right = 0;
//@}


//@{
/** @name Guardan los últimos valores del registro TCNT1 en el momento de saltar la ISR de encoder
 */
volatile uint16_t tcnt1_anterior_left = 0;
volatile uint16_t ultimo_tcnt1_left = 0;

volatile uint16_t tcnt1_anterior_right = 0;
volatile uint16_t ultimo_tcnt1_right = 0;
//@}

//@{
/** @name Almacenan las velocidades calculadas del encoder
 */
volatile float velocidad_left = 0;
volatile float velocidad_right = 0;
//@}

//@{
/** @name Almacenan ultima velocidad de cada encoder. Se usa cuando no hay información nueva (ticks_sin_actualizar > 1)
 */
volatile float ultima_velocidad_left = 0;
volatile float ultima_velocidad_right = 0;
//@}

//@{
/** @name Dirección en la que están girando las ruedas
 *
 * No se utiliza la segunda entrada de encoder debido a que puede pasar mucho tiempo desde que activa la ISR hasta que
 * efectivamente se ejectua, estando la segunda entrada en un valor que no se corresponden con el momento del salto
 */

volatile bool direccion_left = ENCODER_ADELANTE;
volatile bool direccion_right = ENCODER_ADELANTE;
//@}


/**
 * @brief Inicializa pins, interrupciones y resetea variables
 */

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

    encoders_reset_posicion();
    encoders_reset_posicion_total();

}


//@{
/// @name setters y getters
int32_t encoders_get_posicion_total_left(void) {
    return encoder_posicion_total_left;
}

int32_t encoders_get_posicion_total_right(void) {
    return encoder_posicion_total_right;
}

int32_t encoders_get_posicion_aux_left(void) {
    return encoder_posicion_aux_left;
}

int32_t encoders_get_posicion_aux_right(void) {
    return encoder_posicion_aux_right;
}

uint8_t encoders_get_ticks_sin_actualizar_left(void) {
    return ticks_sin_actualizar_left;
}

uint8_t encoders_get_ticks_sin_actualizar_right(void) {
    return ticks_sin_actualizar_right;
}

uint16_t encoders_get_tcnt1_anterior_right() {
    return tcnt1_anterior_right;
}

uint16_t encoders_get_ultimo_tcnt1_right() {
    return ultimo_tcnt1_right;
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

//@}


/**
 * @brief Resetea el contador parcial
 */
void encoders_reset_posicion(void) {
    encoder_posicion_left = 0;
    encoder_posicion_right = 0;
}

/**
 * @brief Resetea el contador total
 */
void encoders_reset_posicion_total(void) {
    encoder_posicion_total_left = 0;
    encoder_posicion_total_right = 0;
}

/**
 * @brief Calcula la distancia recorrida desde el último reset de las posiciones totales
 */
int32_t encoders_get_posicion_total(void) {
    return (encoder_posicion_total_right + encoder_posicion_total_left) / 2;
}

/**
 * @brief Establece la dirección de cada rueda, para saber si sumar o restar en la ISR
 */
void encoders_set_direccion(bool left, bool right) {
    direccion_left = left;
    direccion_right = right;
}

/**
 * @brief Decrementa la posición de ambos encoders un número de pasos
 *
 * @param pasos
 */
void encoders_decrementa_posicion_total(int16_t pasos) {
    encoder_posicion_total_left -= pasos;
    encoder_posicion_total_right -= pasos;
}

/**
 * @brief ISR del encoder izquierdo
 *
 * Se actualizan los contadores respectivos y se guarda el momento (TNCT1) el salto
 */
void encoders_ISR_left(void) {

    ultimo_tcnt1_left = OCR1A * timer1_get_estado() + TCNT1;

    if (direccion_left)
    {
        encoder_posicion_left++;
        encoder_posicion_total_left++;
        encoder_posicion_aux_left++;
    }
    
    else
    {
        encoder_posicion_left--;
        encoder_posicion_total_left--;
        encoder_posicion_aux_left--;
    }
}


/**
 * @brief ISR del encoder izquierdo
 *
 * Se actualizan los contadores respectivos y se guarda el momento (TNCT1) el salto
 */
void encoders_ISR_right(void) {

    ultimo_tcnt1_right = OCR1A * timer1_get_estado() + TCNT1;

    if (direccion_right)
    {
        encoder_posicion_right++;
        encoder_posicion_total_right++;
        encoder_posicion_aux_right++;
    }
    else {
        encoder_posicion_right--;
        encoder_posicion_total_right--;
        encoder_posicion_aux_right--;
    }
}

/**
 * @brief Calcula la velocidad de cada rueda
 *
 * Se calcula la velocidad en función de la diferencia de tiempo entre los dos últimos
 * saltos.
 *
 * @see https://recunchomaker.github.io/adefesio/controlando-la-velocidad/ y posteriores
 */
void encoders_calcula_velocidad() {

    if (encoder_posicion_left == 0) {
        ticks_sin_actualizar_left++;
        velocidad_left = ticks_sin_actualizar_left < 10 ? ultima_velocidad_left : 0;
    }
    else {

        velocidad_left = LONGITUD_PASO_ENCODER * encoder_posicion_left * OCR1A_POR_CICLO /
            (PERIODO_CICLO * ( (int32_t) OCR1A_POR_CICLO * (ticks_sin_actualizar_left + 1) + ultimo_tcnt1_left - tcnt1_anterior_left));

        if ((velocidad_left < 0 and direccion_left == ENCODER_ADELANTE)
            or
            (velocidad_left > 0 and direccion_left == ENCODER_ATRAS))
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

        if ((velocidad_right < 0 and direccion_right == ENCODER_ADELANTE)
            or
            (velocidad_right > 0 and direccion_right == ENCODER_ATRAS))
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

