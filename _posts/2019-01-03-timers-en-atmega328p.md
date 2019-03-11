---
layout: post
title:  "Timers en Atmega328p"
commit: 551ae377134996354cb17f5a4548e88097611b1f
---
El siguiente paso que queremos tomar es tener un control sobre la velocidad a la
que se mueven los motores, es decir, el espacio recorrido por unidad de tiempo.
Un patrón de diseño muy común en este tipo de robots y en otros sistemas
de tiempo real es lanzar una interrupción cada determinado intervalo que
haga las lecturas de sensores necesarias y tome las decisiones correspondientes. Vamos
a seguir este esquema también, utilizando los timers del procesador de Arduino.

## Libreria o código directo?

Existen librerías ya preparadas para utilizar los timers con comandos sencillos. Sin embargo
nosotros vamos a manipular directamente los registros del procesador para configurar el timer.
La razón principal para hacerlo así es para aprender algo por el camino.

> No reinventes la rueda a menos que quieras aprender cómo funcionan las ruedas

## TIMER1 de Arduino

Vamos a utilizar el TIMER1, de 16 bits, que es el que más resolución nos proporciona.
Lo utilizaremos en modo 4 de CTC (Clear Timer on Compare). La diferencia entre los
dos modos CTC de este timer es simplemente el registro que se compara con el
contador de tiempo. En el modo 4 se utiliza el registro OCR1A.

## Prescaler

En una primera aproximación queremos lanzar la interrupción de tiempo cada milisegundo. Como
el reloj del Atmega328p va a 16Mhz, lanzaremos la función cada 16.000 clicks. Según
vemos en la documentación, tendremos que poner el registro de comparacion a 16000 - 1, y
como este número cabe en 16 bits, no necesitamos preescalado.

Según avancemos en el proyecto quizá tengamos que disminuir esta frecuencia, si vemos
que el procesador está muy apurado a medida que aumentemos las funcionalidades. Por el
momento escogemos esta frecuencia porque es bastante redonda

## Primera prueba de timer

Creamos los ficheros timer1.h y timer1.cpp con la funcion timer1_init, que recibe el periodo
en segundos y el preescalado, y hacemos una prueba inicial para un periodo de 1ms sin preescalado.
Escribimos un pequeño programa en el que definimos una ISR que incremente un contador. En el
bucle principal se comprueba si este contador llega a 1000,  e imprime por el puerto serie el total de microsegundos empleados.

```cpp
volatile uint16_t contador = 0;
uint32_t microsegundos = 0;
ISR (TIMER1_COMPA_vect) {
    contador++;
}

void setup() {
    Serial.begin(115200);
    bateria_init();
    motores_init();
    encoders_init();
    bateria_muestra_nivel();
    timer1_init(0.001, 1);
    sei();
    microsegundos = micros();
}

void loop() {
    if (contador > 1000) {
        Serial.println(micros() - microsegundos);
        contador = 0;
        microsegundos = micros();
    }
}
```

Comprobamos con el monitor

```
--- Miniterm on /dev/ttyUSB0  115200,8,N,1 ---
--- Quit: Ctrl+C | Menu: Ctrl+T | Help: Ctrl+T followed by Ctrl+H ---
Bateria: USB
OCR1A = 15999
1000652
1000656
1000656
767652
1000656
1000656
1000656
767652
1000692
1000656
1000648
1000656
1000656
767652
1000700
1000656
```

Vemos que en general se aproxima bastante al segundo, pero de vez en cuando se lanza muchísimo antes,
es decir, la variable contador se incrementa más rápidamente de lo que debiera. Después de darle algunas
vueltas, escribimos en el loop la orden para que imprima la variable contador cuando entra en el if. Sorprendentemente,
cuando hay este desfase de tiempo, la variable vale siempre 768, que es 0x300 en hexadecimal.

La variable "contador" es de 16 bits y el procesador de 8, así que parece que está saltando la interrupción
de tiempo justo en la comparación "if (contador > 1000)". El Atmega no puede hacer esta instrucción en
un único ciclo y compara en dos etapas. En el medio de estas dos etapas se lanza la interrupción y cuando
se vuelve a la segunda etapa de la comparación el valor se ha modificado. En pseudo código sería así:

```cpp
// comparar si el valor en bytes "0x02 0xFF" (767) es mayor que "0x03 0xE8" (1000)
// comparo primero el byte menos significativo
if (0xFF) > (0xE8) go to ETAPA1_TRUE
ETAPA1_TRUE:
if (0x02) >= (0x03) go to RESULT_TRUE
```

El problema es que justo antes de ETAPA1_TRUE se llama a la interrupción, se incrementa el valor
de contador y el segundo byte pasa a valer 0x03. Cuando se retorna el control de la interrupción, 
lo que se ejecuta es:
```cpp
if (0x03) >= (0x03) go to RESULT_TRUE
```

Resumiendo: el timer funcionaba bien: lo que estábamos haciendo mal es NO desactivar las interrupciones
en las instrucciones que pueden llevar varios ciclos y que dependen de variables modificadas en la ISR.

Como comprobación doble, vamos a ejecutar este código en el loop:
```cpp
    if (contador == 1023) {
        Serial.println("1023!");
    }
    cli();
    if (contador > 1000) {
        Serial.println(micros() - microsegundos);
        contador = 0;
        microsegundos = micros();
    }
    sei();
```

Por una parte, vemos que efectivamente aparecen de vez en cuando unos "1023!". Esto ocurre cuando la
variable es 767: la comparación en dos etapas comprueba en una primera fase que el segundo byte es 0xFF,
luego se lanza la interrupción incrementando el segundo byte a 0x03, y a la vuelta de la interrupción
se continúa en la segunda etapa comprobando que, efectivamente 0x03 == 0x03 y dando por True la comparación
total.

Por otra parte, desactivando las interrupciones antes de la comparación, vemos que ahora se comporta
perfectamente mostrando el resultado cada segundo.

## Elaces

[Timers](http://www.avrbeginners.net/architecture/timers/timers.html)

[Timers on the Atmega128/328](https://sites.google.com/site/qeewiki/books/avr-guide/timers-on-the-atmega328)

[Timers in compare mode](http://extremeelectronics.co.in/avr-tutorials/timers-in-compare-mode-part-i/)
