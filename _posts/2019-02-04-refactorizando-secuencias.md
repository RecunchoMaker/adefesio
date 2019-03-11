---
layout: post
title:  "Refactorizando secuencias"
commit: f76b27c50b3b4dce7bac2755d2a805dd36f5b67a
---

Antes de refactorizar el código para crear secuencias de acciones, hemos limpiado un
poco el código y eliminado varias funciones y variables que se nos habían quedado por 
el camino. También eliminamos el sufijo a las variables `_temp` de `motores_actualiza_velocidad`, 
que ya no tenía sentido ese nombre.

En `robot.cpp` hemos creado unas pequeñas funciones para ayudarnos a introducir secuencias de movimientos
de forma fácil.

```cpp
// _         distancia  , aceleracion, deceleracion, velocidad_maxima, velocidad_final, radio
_crea_accion(INFINITO   , 0          , 0.3         , 0.0             , 0              , INFINITO);
_crea_accion(0.18*0.09  , 0.5        , 0.3         , 0.2             , 0              , INFINITO);
_crea_accion((PI*0.09/2), 0.3        , 0.3         , 0.2             , 0.2            , 0.09);
_crea_accion(0.18+0.09  , 0.5        , 0.3         , 0.2             , 0              , INFINITO);
_crea_accion((PI*0.09/2), 0.3        , 0.3         , 0.2             , 0.2            , 0.09);
_crea_accion(0.18+0.09  , 0.5        , 0.3         , 0.2             , 0              , INFINITO);
_crea_accion((PI*0.09/2), 0.3        , 0.3         , 0.2             , 0.2            , 0.09);
_crea_accion(0.18+0.09  , 0.5        , 0.3         , 0.2             , 0              , INFINITO);
```

La primera acción, con aceleración y velocidad 0, es simplemente la acción de inicio que mantiene
el robot parado y, cuando se alcanza la última, se vuelve a esta acción 0 y se paran los motores

Esto sólo servirá en las pruebas. En un uso real tendremos que transformar un string
de tipo `AADAAI` a una o varias acciones de este tipo. Lo que hace la secuencia de arriba es
símplemente hacer un cuadrado "redondeado", es decir, avanza el equivalente a casilla y media,
y hace un giro de 90 grados (radio 9 cm) cuatro veces, hasta volver a la "casilla" de salida.

Hemos puesto un montón de _Serial.print_ para controlar lo que está haciendo antes de afinar
el pid con varios movimientos combinados. Se muestra la posición de los encoders antes de lanzar
la acción siguiente, mostrando los cálculos y cuándo decelera.

```
GO!
Encoders: 21 (0.01m), 23 (0.01m)
---- siguiente accion: 1
0.27m (698 pasos) r=-1.00m - acc/dec:0.50/0.30m/ss ; decelerar en 526 (0.20m.) ; v max: 0.20m/s
decelero en 0.07=0.20m - vel=0.22
Alcanzo pasos 698 a 0.14 m/s
Encoders: 698 (0.27m), 700 (0.27m)
---- siguiente accion: 2
0.14m (365 pasos) r=0.09m - acc/dec:0.30/0.30m/ss ; decelerar en 365 (0.14m.) ; v max: 0.20m/s
Alcanzo pasos 365 a 0.20 m/s
Encoders: 529 (0.08m), 201 (0.08m)
---- siguiente accion: 3
0.27m (698 pasos) r=-1.00m - acc/dec:0.50/0.30m/ss ; decelerar en 526 (0.20m.) ; v max: 0.20m/s
decelero en 0.07=0.20m - vel=0.22
Alcanzo pasos 698 a 0.20 m/s
Encoders: 708 (0.27m), 691 (0.27m)
---- siguiente accion: 4
0.14m (365 pasos) r=0.09m - acc/dec:0.30/0.30m/ss ; decelerar en 365 (0.14m.) ; v max: 0.20m/s
Alcanzo pasos 365 a 0.20 m/s
Encoders: 526 (0.08m), 208 (0.08m)
---- siguiente accion: 5
0.27m (698 pasos) r=-1.00m - acc/dec:0.50/0.30m/ss ; decelerar en 526 (0.20m.) ; v max: 0.20m/s
decelero en 0.07=0.20m - vel=0.20
Alcanzo pasos 698 a 0.20 m/s
Encoders: 706 (0.27m), 690 (0.27m)
---- siguiente accion: 6
0.14m (365 pasos) r=0.09m - acc/dec:0.30/0.30m/ss ; decelerar en 365 (0.14m.) ; v max: 0.20m/s
Alcanzo pasos 365 a 0.20 m/s
Encoders: 522 (0.08m), 209 (0.08m)
---- siguiente accion: 7
0.27m (698 pasos) r=-1.00m - acc/dec:0.50/0.30m/ss ; decelerar en 526 (0.20m.) ; v max: 0.20m/s
decelero en 0.07=0.20m - vel=0.20
Alcanzo pasos 698 a 0.20 m/s
Encoders: 709 (0.27m), 691 (0.27m)
---- siguiente accion: 0
-1.00m (4294967295 pasos) r=-1.00m - acc/dec:0.00/0.30m/ss ; decelerar en 4294967295 (-1.00m.) ; v max: 0.20m/s
```

El robot Hace algo parecido a lo que le pedimos, aunque no llega a cerrar el cuadrado (los giros no
acaban de ser correctos). Según el log, vemos varias cosas que tendremos que depurar en los próximos
días:

1. El robot no decelera bien: aún estableciendo una velocidad final de 0 en las rectas llega a la siguiente acción 
a gran velocidad
2. El giro no es correcto. Antes estábamos viendo el recorrido de la rueda exterior (en la que tendríamos
mayor precisión): ahora lo estamos haciendo con la media de los dos encoders. Un mundo ideal debería
funcionar pero parece que no es así.

En cuanto arreglemos eso podremos encontrar mejores valores para el pid, así como definir las constantes
de aceleración para rectas y curvas, velocidades de entrada y de salida... etc.
