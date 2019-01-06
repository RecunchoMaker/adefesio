---
layout: post
title:  "Controlando la velocidad - caso 2"
---
Vamos a intentar a avanzar depurando el cálculo para el casao 2, a ver
si en el camino conseguimos más información que nos ayude a entender mejor
el caso 1. Lanzamos una prueba y extraemos únicamente los valores de este
caso.

Viendo únicamente los resultados para el pwm de 160 detectamos varios
errores.

```
~$ [master>] pio device monitor
Bateria: USB
OCR1A = 15999
pwm = 160
caso:3   sin act:99   encoder left: 0   ticks: 27036   0,1 = 0 0
caso:2   sin act:1   encoder left: 1   ticks: 32777   0,1 = 5946 1013
caso:2   sin act:1   encoder left: 1   ticks: 27581   0,1 = 2335 2713
caso:2   sin act:0   encoder left: 1   ticks: 742   0,1 = 15288 31
caso:1   sin act:1   encoder left: 2   ticks: 11221   0,1 = 4144 15365
```

En primer lugar, suponiendo quee el caso 1 está correcto (última línea), los ticks en
el caso 2 deberían ser también sobre esta cifra. Recordemos cuándo se produce el 
caso 2:

```
      +-----+         +-----+         +-----+         +-----+         +
      |     |         |     |         |     |         |     |         |  
      | 1ms |         | 2ms |         | 3ms |         | 4ms |         |  
    --+     +---------+     +---------+     +---------+     +---------+
Caso 2:       1     2           3                              4

```
En el tercer milisegundo, cuando se lanza la interrupción de tiempo y calculamos la
velocidad, el encoder ha saltado una única vez (salto 3) desde el último reseteo (en el milisegundo 2)
y en el buffer `tcnt1_left[]` tendremos los valores de TNCT1 del salto 2 y del salto 3. 

Para el milisegundo 3, la velocidad sería `tnct1_left[1] - tnct1_left[0] + OCR1A', dado
que el primer valor del bucle se corresponde con el ciclo anterior, y hay que sumar dicha
longitud de ciclo. En el milisegundo 5, que tenemos igualmente el caso 2, el cálculo sería
`tcnt1_left[1] - tcnt1_left[0] + 2 * OCR1A, ya que el valor contenido en tcnt1_left[0] es
el de dos ciclos antes.

Vemos que estamos incrementando erróneamente la variable `ticks_sin_actualizar_left`. En el
`if` del caso 2 la inicializamos a 1, pero no es correcto. Realmente debe inicializarse a 0. Lo
hacemos y repetimos la prueba.

```
Bateria: USB
OCR1A = 15999
pwm = 160
caso:3   sin act:99   encoder left: 0   ticks: 27036   0,1 = 0 0 
caso:2   sin act:0   encoder left: 1   ticks: 16273   0,1 = 12266 12540 
caso:1   sin act:0   encoder left: 2   ticks: 11504   0,1 = 2637 14141 
caso:2   sin act:0   encoder left: 1   ticks: 11098   0,1 = 3856 4619 
caso:2   sin act:0   encoder left: 1   ticks: 11242   0,1 = 243 562 
pwm = 161
caso:1   sin act:0   encoder left: 2   ticks: 11366   0,1 = 3815 15181 
caso:1   sin act:0   encoder left: 2   ticks: 11392   0,1 = 1945 13337 
caso:1   sin act:0   encoder left: 2   ticks: 11176   0,1 = 14032 13882 
caso:2   sin act:0   encoder left: 1   ticks: 15881   0,1 = 10926 6172 
caso:1   sin act:1   encoder left: 2   ticks: 10762   0,1 = 216 10978 
pwm = 162
caso:2   sin act:0   encoder left: 1   ticks: 16480   0,1 = 8389 3353 
caso:2   sin act:0   encoder left: 1   ticks: 10812   0,1 = 12950 7763 
caso:2   sin act:0   encoder left: 1   ticks: 10756   0,1 = 10189 9957 
caso:2   sin act:0   encoder left: 1   ticks: 15807   0,1 = 11682 6835 
caso:2   sin act:0   encoder left: 1   ticks: 11094   0,1 = 2523 2213 
```

Mucho mejor, aunque hay algo (que ya pasaba en el caso 1) que no nos acaba de encajar: los
valores de ticks para estos pwm cercanos (160-162) oscilan aleatoriamente entre dos grupos:
alrededor de 16000 y alrededor de 11000. Es una diferencia del 50%!

# Logeando todos los datos del encoder

Con el arduino no vamos a poder (o al menos no se nos ocurre por ahora cómo hacerlo) logear
en tiempo real todos los datos en cada salto de encoder... así que, aun a riesgo de aumentar los tiempos de
proceso de la interrupción y agravar el probema, vamos a intentar guardar en un array un
histórico de ticks en la interrupción de reloj y mostrarlos cuando el buffer se llena.

```
    ticks = encoders_get_ticks_entre_saltos_left();
    log_buffer[log_buffer_idx++] = ticks;
    if (log_buffer_idx == 255) 
        for (log_buffer_idx = 0; log_buffer_idx < 255; log_buffer_idx++)
            Serial.println(log_buffer[log_buffer_idx]);
```

Esto lanza un montón de datos pero vemos un patrón que nos da una pista de lo que puede estar pasando:

```
15887
11002
16350
11012
10817
16451
10787
15953
11206
11327
```
Vemos que hay prácticamente alternancia entre los valores cercanos a 16000 y valores cercanos a 11000. Creemos
que puede ser debido a que el encoder de cuadratura del motor NO mantiene el valor alto y bajo del voltaje al 50%,
si no que está precisamente en esa relación: aproximadamente 15/10. En su momento configuramos la interrupción
del encoder izquierdo en modo CHANGE pero el encoder tarda menos en saltar de HIGH a LOW
que de LOW a HIGH lanzando estas tiempos diferentes de una vez a otra, aunque la rueda esté girando a la misma
velocidad. Hay una manera muy sencilla de comprobarlo. Configuramos la interrupción a RISING y relanzamos la prueba.

```
Bateria: USB
OCR1A = 15999
pwm = 160
caso:2   sin act:0   encoder left: 1   ticks: 27315   0,1 = 2727 14043 
caso:2   sin act:1   encoder left: 1   ticks: 27207   0,1 = 1202 12361 
caso:2   sin act:1   encoder left: 1   ticks: 27225   0,1 = 6936 2163 
pwm = 161
caso:2   sin act:1   encoder left: 1   ticks: 27042   0,1 = 989 11902 
caso:2   sin act:1   encoder left: 1   ticks: 26719   0,1 = 7289 2010 
caso:2   sin act:1   encoder left: 1   ticks: 26940   0,1 = 10838 5780 
caso:2   sin act:1   encoder left: 1   ticks: 26799   0,1 = 10883 5684 
caso:2   sin act:1   encoder left: 1   ticks: 27048   0,1 = 10145 5195 
caso:2   sin act:0   encoder left: 1   ticks: 26957   0,1 = 2386 13344 
caso:2   sin act:0   encoder left: 1   ticks: 26865   0,1 = 5107 15973 
pwm = 162
caso:2   sin act:0   encoder left: 1   ticks: 26732   0,1 = 4139 14872 
caso:2   sin act:0   encoder left: 1   ticks: 26707   0,1 = 553 11261 
caso:2   sin act:1   encoder left: 1   ticks: 26676   0,1 = 10813 5491 
caso:2   sin act:1   encoder left: 1   ticks: 26621   0,1 = 6459 1082 
caso:2   sin act:0   encoder left: 1   ticks: 26755   0,1 = 3540 14296 
caso:2   sin act:1   encoder left: 1   ticks: 26652   0,1 = 13832 8486 
caso:2   sin act:0   encoder left: 1   ticks: 26720   0,1 = 2423 13144 
caso:2   sin act:0   encoder left: 1   ticks: 26498   0,1 = 1446 11945 
pwm = 163
caso:2   sin act:1   encoder left: 1   ticks: 26795   0,1 = 12618 7415 
caso:2   sin act:1   encoder left: 1   ticks: 26361   0,1 = 11663 6026 
caso:2   sin act:1   encoder left: 1   ticks: 26644   0,1 = 10232 4878 
caso:2   sin act:1   encoder left: 1   ticks: 26502   0,1 = 11966 6470 
caso:2   sin act:1   encoder left: 1   ticks: 26385   0,1 = 14285 8672 
caso:2   sin act:1   encoder left: 1   ticks: 26434   0,1 = 10957 5393 
caso:2   sin act:1   encoder left: 1   ticks: 26607   0,1 = 7764 2373 
pwm = 164
caso:2   sin act:1   encoder left: 1   ticks: 26553   0,1 = 6742 1297 
caso:2   sin act:1   encoder left: 1   ticks: 26262   0,1 = 10843 5107 
caso:2   sin act:1   encoder left: 1   ticks: 26224   0,1 = 13575 7801 
caso:2   sin act:1   encoder left: 1   ticks: 26318   0,1 = 13409 7729 
caso:2   sin act:1   encoder left: 1   ticks: 26373   0,1 = 8602 2977 
pwm = 165
caso:2   sin act:0   encoder left: 1   ticks: 26340   0,1 = 2691 13032 
caso:2   sin act:1   encoder left: 1   ticks: 26005   0,1 = 15344 9351 
caso:2   sin act:0   encoder left: 1   ticks: 25971   0,1 = 3690 13662 
caso:2   sin act:0   encoder left: 1   ticks: 26116   0,1 = 5065 15182 
caso:2   sin act:1   encoder left: 1   ticks: 25886   0,1 = 10764 4652 
caso:2   sin act:1   encoder left: 1   ticks: 26133   0,1 = 11378 5513 
caso:2   sin act:0   encoder left: 1   ticks: 26044   0,1 = 4812 14857 
```

Eureka! Efectivamente, algo avanzamos: los valores son ahora muchísimo más estables. Por otra parte, perdemos
la mitad de resolución en el encoder (saltamos sólo la mitad de las veces, que ya eran pocas inicialmente).
El caso 1 ya no nos ha aparecido nunca en la prueba.

Continuaremos mañana con el caso 3.

## Disclaimer sobre este commit

Este commit está especialmente _sucio_. Tendríamos tiempo para limpiarlo pero preferimos subirlo así
precisamente para dejar constancia de las pruebas y la basura temporal que se ha hecho. Una vez acabada
esta "serie" sobre la velocidad, dejaremos todo más limpio.

commit 8fca3932d4e47dcd2660061ba7228222691b85d6

