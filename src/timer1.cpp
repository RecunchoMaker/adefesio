#include <Arduino.h>
#include <timer1.h>

volatile uint32_t cuenta;
volatile int8_t estado;

void timer1_init(float period, uint16_t prescaler) {

    /*
     * http://www.avrbeginners.net/architecture/timers/timers.html#timsk_tifr
     * https://sites.google.com/site/qeewiki/books/avr-guide/timers-on-the-atmega328
     *
     ********************************************************************
     * TCCR1A
     ********************************************************************
     * bit       7      6      5      4      3      2      1      0
     *        COM1A1 COM1A0 COM1B1 COM1B0    -      -    PWM11  PWM12
     *
     *        COM1A: Compare Output Mode : controla si y como el Compare
     *               Output pin se conecta al timer 1
     *           0      0    Disconnect Pin OC1 from Timer/Counter 1
     *           0      1    Toggle OC1 on compare match
     *           1      0    Clear OC1 on compare match
     *           1      1    Set OC1 on compare match
     ********************************************************************
     * TCCR1B
     ********************************************************************
     * bit       7      6      5      4      3      2      1      0
     *        ICNC1   ICES1    -    WGM13  WGM12  CS12   CS11   CS10
     *
     * Modos
     * WGM13 WGM12 WGM11 WGM10
     *   0     1     0     0     Modo 4: CTC con OCR1A 
     *   1     1     0     0     Modo 12: CTC con ICR1
     *        
     * Preescalers
     * CSn2 CSn1 CSn0
     *   0    0    0   No clock source
     *   0    0    1   clk/1 (no preescale)
     *   0    1    0   clk/8
     *   0    1    1   clk/64
     *   1    0    0   clk/256
     *   1    0    1   clk/1024
     *   1    1    0   fuente reloj externa (falling)
     *   1    1    1   fuente reloj externa (rising)
     *
     ********************************************************************
     * TIMSK (Timer/Counter Interrupt Mask Register)
     ********************************************************************
     * bit       7      6      5      4      3      2      1      0
     *                       ICIE1    -      -    OCIE1B OCIE1A TOIE0
     *
     ********************************************************************
     * OCR1 (Output Compare Register)
     ********************************************************************
     * The Output Compare register can be used to generate an
     * Interrupt after the number of clock ticks written to it.
     * It is permanently compared to TCNT1. When both match, the
     * compare match interrupt is triggered. If the time between
     * interrupts is supposed to be equal every time, the CTC bit
     * has to be set (TCCR1B). It is a 16-bit register
     * OCRn =  [ (clock_speed / Prescaler_value) * Desired_time_in_Seconds ] - 1
     *      =  [ 16000000 * periodo / preescalado ] 1
     */

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

    cuenta = 0;
    estado = 0;
}

void timer1_incrementa_cuenta() {
    cuenta++;
    estado++;
    if (estado == 4) estado = 0;
}

void timer1_reset_cuenta() {
    cuenta=0;
}

uint32_t timer1_get_cuenta() {
    return cuenta;
}

int8_t timer1_get_estado() {
    return estado;
}
