---
layout: post
title:  "Encendiendo y apagando"
commit: 0648f4186211ea8f3da72a8a53592ecf050ac2ea
---

Vamos a probar el circuito por partes, pero empezamos con un led normal y una R de 220 ohm, para
no andar con la cámare del móvil comprobando si se enciende el infrarrojo. Queremos ser capaces
de controlar el tiempo que se encienden los leds: se trata de encontrar el mínimo 
necesario para alcanzar la intensidad máxima.

Por cierto, no tenemos otro Arduino nano, así que para no quitar del zócalo de _Adefesio_, usamos
un UNO que tenemos por aquí: por eso hay otro entorno de compilación para el UNO en el platformio.ini.

# Control de tiempo por TCNT1

Creamos los ficheros `leds.h` y `leds.cpp`, de la manera habitual, y metemos en
el timer en 'main()', el siguiente código:

```cpp
aux_1f = TCNT1;
digitalWrite(LED_IZQ, HIGH);
while (TCNT1 < aux_1f + tiempo_espera);
digitalWrite(LED_IZQ, LOW);
```

Encendemos, esperamos a que el contador de timer avance unos cuantos pasos y
apagamos. Vamos subiendo el valor en la comparación, pero con un valor de 500 el led alumbra poquísimo. Esta
manera implica mucho tiempo perdido, que va a estorbar al cálculo de velocidad.

Ni siquiera poniendo el encendido al principio de la interrupción y el apagado al final,
después de todo el proceso que tenemos actualmente, logramos una intensidad decente. Así
que tendremos que encenderlo en un salto de timer y apagarlo en otro.

# Máquina de estados

```cpp
timer_estado++;

switch (timer_estado & B00000011) {
    case 0: digitalWrite(LED_IZQ, HIGH);
            break;
    case 1: digitalWrite(LED_IZQ, LOW);
            break;
    case 2: 
            break;
    case 3: 
            break;
}
```

Ahora sí que se enciende con bastante intensidad, aunque tenemos un periodo de 4ms y un
ciclo de trabajo del 25%. Con nuestro diodo infrarrojo, estaríamos al límite de las posibilidades
del datasheet.

# Volviendo al periodo de 1ms

Ahora no nos va a dar tiempo, pero lo que queremos hacer es usar esta máquina de estados para las
demás funciones, no sólo para el encendido/apagado de los leds. Habra que experimentar con distinto
número de estados y repartiremos el trabajo de las demás funciones incluidas
en el timer entre los distintos estados. La frecuencia de lectura de los encoders será igualmente cada
4 ms (por ejemplo en el estado 0), y habra crear una nueva variable `número de estados` para que
las funciones de cálculo de velocidad en encoders tenga siempre en cuenta que el tiempo ya no es
TNCT1, si no OCR1A * (número de estados - 1) + TNCT1.
