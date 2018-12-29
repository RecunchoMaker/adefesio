---
layout: post
title:  "Motores y puente H"
---

## Puente H
Para controlar los motores utilizaremos el puente H [TB6612FNG](https://tienda.bricogeek.com/controladores-motores/999-controlador-de-motores-tb6612fng.html)
, que es relativamente barato y tiene
unas prestaciones suficientes para un proyecto educativo como este. Nos hemos dejado 
guiar por la elección de nuestros amigos de Bricolabs para el [raptor](https://bricolabs.cc/wiki/proyectos/raptor), 
un robot siguelíneas de bajo coste.

Este puente H permite controlar cada motor con 3 pines del arduino: un pin recibe la señal
de PWM y con los otros dos se codifican las cuatro situaciones posibles que admite: mover
el motor hacia adelante, mover el motor hacia atrás, bloquear el motor (parada brusca) y
liberar el motor.

## Motores
Necesitamos unos motores con encoders, que sean baratos y fáciles de conseguir. Finalmente
nos han prestado estos:

[MicroMetal geared motor with encoder](http://www.volumerate.com/product/449751)

A pesar de que no son excesivamente rápidos, sí que tienen una buena aceleración, con lo cual 
tendremos que aprender a controlar el arranque y la frenada, que es lo que nos interesa inicialmente.

## Control de pwm
Vamos a crear el fichero "motores.h" y "motores.cpp" en los que incluiremos las funciones
necesarias para controlar desde el arduino la potencia que el puente H entrega a los motores.
Utilizaremos un control PWM. Crearemos las funciones motores_set_pwm e motores_init.

A pesar de que en una primera aproximación vamos a utilizar código C puro, al usar
el framework de Arduino, que es un proyecto en cpp, crearemos los ficheros de tipo ".cpp" para
facilitar las cosas con la inclusión de las librerías de arduino.

Los valores PWM están en el rango de 0 a 255, pero vamos a considerar también pwm negativos
(cuando el motor gira hacia atrás). Así que utilizamos enteros de 16 bits con signo. Utilizaremos
los PWM del arduino en los pines D5 y D6
PWM del arduino

Realizamos las siguientes conexiones entre el arduino, el puente H y los motores. No usaremos
el pin de STBY para activar o desactivar el puente H. Siempre estará activado.

| arduino       | puente H  | Motor A | Motor B |
|:--------------|:----------|:--------|:--------|
| VCC           | VCC,STBY  |         |         |
| Vin           | VM        |         |         |
| GND           | GND       |         |         |
| D5            | PWMA      |         |         |
| D9            | AIN2      |         |         |
| D4            | AIN1      |         |         |
| D6            | PWMB      |         |         |
| D7            | BIN2      |         |         |
| D10           | BIN1      |         |         |
|               | A01       |   I1    |         |
|               | A02       |   I2    |         |
| D10           | AIN1      |         |  I1     |
| D10           | BIN1      |         |  I2     |

Hacemos un pequeño script de prueba en la funcion loop de main.cpp

```cpp
for (int16_t i = -200; i < 200; i++) {
    Serial.println(i);
    motores_set_pwm(i, -i);
    delay(100);
}
```

Las ruedas giran en sentido contrario como era de esperar. A un pwm de 30-35 se empiezan a mover,
 y con pwm de 200, la velocidad aproximada es de 2 vueltas por segundo (teniendo en cuenta que 
estamos alimentando los motores con los 5v del USB del ordenador y el motor corriendo en vacío,
simplemente con la rueda)

## Links:
* [Controlar dos motores DC con Arduino y driver TB6612FNG](https://www.luisllamas.es/arduino-motor-dc-tb6612fng/)

* [Comprar TB6612FNG](https://tienda.bricogeek.com/controladores-motores/999-controlador-de-motores-tb6612fng.html)

_commit_: 986422e520c65bc79119adf60770a9d7ef00067d
