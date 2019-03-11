---
layout: post
title:  "Segmento en recta y parar"
commit: 201a9c32995c9dc04cdfa4f30cdcf30c0340126a
---

Intantaremos realimentar el pid con la información de las lecturas de sensores. Vamos
a empezar con lo que parece más fácil: cuando el robot se encuentra entre dos pareces laterales
yendo en línea recta. Se corresponde en el gráfico de ayer con la primera rampa roja (la
que indica la posición del encoder hasta su reseteo al entrar en la segunda acción). La
segunda celda también tiene dos paredes laterales, pero al estar los sensores adelantados,
y un poco en diagonal, la lectura del sensor derecho cae, antes de llegar al final de la
casilla. Ya veremos cómo afrentar esto. Ahora simplemente vamos a intantar que el robot
vaya recto entre un pasillo de paredes.


# Prueba de ajuste de pid por diferencia de valor en sensores laterales

Lo más sencillo que se nos ocurre en un pasillo es minimizar la diferencia entre los
sensores laterales, sin hacer caso (por ahora) a la distancia absoluta que representan.
Hacemos una pequeña función `led_get_desvio_centro()` que nos devuelve la diferencia
de valores entre ambos sensores, y ajustamos el pid en `motores.cpp` con:

```cpp
potencia_left += 0.0001 * leds_get_desvio_centro();
potencia_right -= 0.0001 * leds_get_desvio_centro();
```

Este valor está puesto como primera aproximación, aunque si seguimos por este camino, será
una constante Kp, e incluso podríamos introducir la componente integral.

El robot va perfectamente recto entre un pasillo que nos hemos hecho de 6 casillas. Incluso
empezando con un giro de 45º se endereza inmediatamente.


# Pararse delante de una pared

Seguimos experimentando con los sensores. Queremos detenernos al encontrar una pared (por ahora,
siempre en un pasillo con paredes a ambos lados).  Hemos cambiado completamente la función `robot_siguiente_accion()`
para que, en vez de introducir un conjunto de pasos inicialmente, decida la siguiente acción en función
del estado del robot y alguna otra cosa más.

Actualmente, con nuestra división en _acciones_ que se ejecutan una por casilla, lo que hacemos
es _mirar_ si los sensores frontales superan cierto valor (ahora mismo hemos puesto 30). Así decidimos
si continuamos yendo recto o ejecutamos la acción de pararse en mitad de la casilla.

Todo esto funciona pero no nos gusta nada la pinta que va cogiendo el código. Seguiremos experimentando
con curvas de 90º y curvas en _U_, pero definitivamente habrá que tirar buena parte del código en `robot.cpp`.
