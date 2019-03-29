/**
 * @file timer1.cpp
 *
 * @brief Control del TIMER1 de arduino
 *
 * Se utiliza el TIMER1 en modo CTC
 *
 * @see https://recunchomaker.github.io/adefesio/timers-en-atmega328p
 * @see http://www.avrbeginners.net/architecture/timers/timers.html
 * @see https://sites.google.com/site/qeewiki/books/avr-guide/timers-on-the-atmega328
 */
#include <Arduino.h>
#include <timer1.h>
#include <settings.h>

volatile uint32_t timer_cuenta;
volatile int8_t timer_estado;

/**
 * @brief Inicializa los registros necesarios para la interrupción de tiempo
 *
 * @param period periodo del timer en segundos
 * @param prescaler constante de preescalado del timer (1,8, 256 o 1024)
 */
void timer1_init(float period, uint16_t prescaler) {

    // borramos registro de control timer1 (TimerCounter Control Register)
    TCCR1A = 0;
    TCCR1B = 0;

    switch (prescaler) {
    case 1:
        TCCR1B |= (1 << CS10);
        break;
    case 8:
        TCCR1B |= (1 << CS11);
        break;
    case 256:
        TCCR1B |= (1 << CS12);
        break;
    case 1024:
        TCCR1B |= (1 << CS12) | (1 << CS10);
        break;
    default:
        Serial.println("Error en prescaler");
        TCCR1B |= (1 << CS10);
        break;
    }

    // asignamos el valor del registro de comparacion
    uint32_t res = ((16000000.0 / prescaler ) * (period)) - 1;

    OCR1A = res;

    // Asignamos el modo 4, activar CTC si timer = OCR1A
    TCCR1B |= (1 << WGM12);

    // Asignamos la mascara de interrupcion a "Compare Match A"
    TIMSK1 |= (1 << OCIE1A);

    timer_cuenta = 0;
    timer_estado = 0;
}


/**
 * @brief Getter de timer_cuenta
 */
uint32_t timer1_get_cuenta() {
    return timer_cuenta;
}


/**
 * @brief Getter de timer_estado
 */
int8_t timer1_get_estado() {
    return timer_estado;
}


/**
 * @brief Resetea la cuenta de entradas en la interrupción
 */
void timer1_reset_cuenta() {
    timer_cuenta=0;
}


/**
 * @brief Incrementa el contador de entradas en la interrupción y gestiona el estado
 *
 * Es necesario incluir esta función en la rutina de interrupción del timer, para
 * poder llevar la cuenta de las veces que se ha entrado (desde el último reseteo).
 * También gestiona un número estado secuencial cíclico entre 0 y NUMERO_ESTADOS-1
 */
void timer1_incrementa_cuenta() {
    timer_cuenta++;
    timer_estado++;
    if (timer_estado == NUMERO_ESTADOS) timer_estado = 0;
}
