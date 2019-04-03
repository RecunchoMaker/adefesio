---
layout: post
title:  "Frenada con pared frontal y corrección lateral"
commit: 0b3fb2d073ff3c157a9b745cafb407979b214cc1
---

Ahora que ya tenemos varias cosas medio listas, montamos de nuevo un pequeño
laberinto de 5x4, quitamos todos los controles de corrección, y vemos cómo se comporta.
La idea es ir evaluando qué parámetros son más críticos e irlos afinando poco a poco.

El laberinto para estas pruebas es así:

```
+----+----+----+----+
|  0    1    2 |  3 |
+    +----+    +    +
|  4 |  5    6 |  7 |
+    +----+    +    +
|  8    9 | 10 | 11 |
+    +----+    +    +
| 12 | 13   14 | 15 |
+    +----+    +    +
| 16   17   18   19 |
+----+----+----+----+
```

El robot tendría que hacer el camino: 16-12-8-4-0 (flood) 1-2-6-10-14-18 (flood) 19-15-11-7-3. Pero
lógicamente, sin ningún tipo de sincronización acaba chocando con alguna pared antes de llegar.
Generalmente en la esquina que forman las casillas 5-10, o a lo largo de ese pasillo que va de
2 a 18. 

# Frenada en pared central

Lo primero que vemos es la necesidad de sincronizar la frenada y tener en cuenta los sensores para parar
a una distancia conocida de la pared. Ahora mismo, el robot para un poco adelantado o atrasado, lo que
hace que al hacer el giro, a poco que se desvíe, llega a la casilla 2 bastante torcido. Este error se
va acumulando y el pasillo hasta 18 casi nunca lo consigue hacer.

El control que vamos a definir para esto es que cuando nos acercamos a una pared (estamos en la acción
PARAR) la velocidad es proporcional a la distancia al punto en el que queremos parar (aproximadamente
a unos 19mm, que sumados a la distancia de los sensores al centro del robot, nos da el centro de la
casilla).

```cpp
if (accion_get_accion_actual() == PARA and leds_pared_enfrente()) {
    motores_set_velocidad_lineal_objetivo( 
            min(
            ACCION_V0 + ((leds_get_distancia(LED_FDER)+leds_get_distancia(LED_FIZQ)/2.0) - 0.019) * (ACCION_VE/(ACCION_VE-ACCION_V0)),
            ACCION_VE));
    accion_set_pasos_objetivo(pasos_recorridos + (( (leds_get_distancia(LED_FIZQ) + leds_get_distancia(LED_FDER)) / 2.0)-0.019) / LONGITUD_PASO_ENCODER);
```

Con esto conseguimos al menos que en la primera parada en 0, el robot está alineado en primera fila.
Aunque sigue desviado del centro de la columna


# Corrección de giro en función de pasos del encoder

En `encoders.cpp` tenemos un contador de pasos auxiliar que utilizábamos de vez en cuando para logs.
Hemos decidido utilizarlo para llevar la cuenta de los pasos totales entre arrancada y frenada, y
tomarlo como indicador del ángulo que se ha desfasado el robot con respecto al arranque. Y en los
giros de 90 añadimos (o retamos) este error a la distancia que tiene que girar.

Después de probar esto, no nos acaba de convencer demasiado... habrá que desarrollarlo un poco más
matemáticamente y ver si compensa o no. Hay que tener el cuenta que, si el robot inicia la recta
con un ángulo torcido, y se consigue corregir este error al final, la cuenta de encoders estará
desfasada y este cálculo retrocederá la corrección.

# Corrección de offset

Lo que está ocurriendo ahora es que el robot llega a la casilla 0 ligeramente desviado a la derecha o a la izquierda.
Este error se va acumulando en distintos giros, trasladándose del eje x al eje y, por ejemplo: si estamos
desviados a la izquierda, al girar 90 grados hacia la casilla 1 y avanzar las dos casillas, nos pararemos
un poco antes de lo debido (o un poco después si el desvío original es a la izquierda). 

Esto no ha afectado a nada en este laberinto ya que siempre tenemos una pared frontal al hacer el giro. Lo
miraremos más tarde

# Preferencia de dirección en cálculo de flood

Aún con todos estos errores (y afinando un poco la corrección de ángulo en rectas, que definitivamente tenemos 
que replantear) a veces el robot consigue llegar a la solución. Se hace unas sucesivas paradas/rearrancadas
en las casillas 6, 10, 14 y 18. Como sólo hacemos flood cuando estamos parados, el cálculo de camino que se
hace en la casilla 2 pasa por 6-7-3 (ya que aún no se conoce la pared entre 6 y 7). Así que hay que cortar el
camino, parar, recalcular... etc. A la hora de calcular el camino, si las casillas no están visitadas, tenemos que continuar
recto hasta encontrar una pared conocida.

