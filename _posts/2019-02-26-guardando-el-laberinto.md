---
layout: post
title:  "Guardando el laberinto"
commit: fee8fb6db9f416281ae3cef61b70d6eb5f49c204
---

Antes de seguir con los sensores vamos a definir la estructura de datos en la que
guardaremos el laberinto.

El objetivo final es utilizar un algoritmo relativamente óptimo en la exploración,
que no explore inútilmente zonas de laberinto que claramente no pueden conducir
a la solución (como ocurre con el algoritmo de la "mano derecha"). Tendremos necesariamente
que tener una matriz que almacene las paredes de las celdas exploradas y algún
dato más.

# Ahorrar memoria o tiempo?

El laberinto se puede almacenar en una matriz de 16x16. Si reservamos un bit por
pared, duplicamos la información ya que coinciden las paredes Norte y Sur de
celdas adyacentes verticalmente (y Este/Oeste en sentido horizontal). Así que sólo necesitamos
dos bits por celda para almacenar las paredes.

```cpp
typedef struct {
    char paredN : 1;
    char paredE : 1;
} tipo_celda;
```

Lógicamente esto ocupa igualmente un byte por celda. Pero podemos añadir 6 bits más "gratis".

Si creamos un array de filas x columnas, tendremos que poner cierta lógica en la
comprobación de paredes, ya que la pared _SUR_ de las celdas más al sur no estaría en
la matriz (lo mismo que la celda _OESTE_ de las celdas más al oeste). Asi que nuestra
matriz tendrá una columna y una fila _dummy_, para ahorrar tiempo en las comprobaciónes
que podamos hacer.

# Imprimir el laberinto

Creamos los ficheros `laberinto.h` y `laberinto.cpp`, con las funciones de inicialización
y una pequeña rutina que nos muestre el laberinto. Este es un ejemplo de un laberinto 4x5.

```
+-----+-----+-----+-----+-----+
|   0     1     2     3     4 |
|                             |
+     +     +     +     +     +
|   6     7     8     9    10 |
|                             |
+     +     +     +     +     +
|  12    13    14    15    16 |
|                             |
+     +-----+     +     +     +
|  18 |  19    20    21    22 |
|     |                       |
+-----+-----+-----+-----+-----+
A>
```

En vez de una matriz escogemos un array unidimensional (también para ahorrarnos algo de lógica de
proceso, ya que podemos desplazarnos derecha/izquerda sumando y restando 1, o norte y sur sumando
o restando el número de columnas + 1. Establecemos el convenio que la primera celda está en la
parte superior izquierda. Las paredes en la celda 19 son simplemente una prueba.

Los números que se muestran se corresponden con el número de celda (salta del 4 al 6 porque hay
la columna _dummy_). Esto es sólo para referencia. Cuando empezemos con algún algoritmo de resolución
se mostrarán los bits implicados y algún valor, así como la posición/orientación del robot.

# Memoria disponible

Hemos puesto un par de variables `char` en la estructura `tipo_celda` para ver que margen de memoria
tenemos creando una matriz de 17x17 (laberinto de 16x16 con la fila/columna adicional). Realmente
el tamaño es 17x17-1, ya que no es necesaria la celda de la columna _dummy_ de la fila _dummy_.

```
Checking size .pioenvs/nanoatmega328/firmware.elf
Memory Usage -> http://bit.ly/pio-memory-usage
DATA:    [========  ]  84.3% (used 1730 bytes from 2048 bytes)
PROGRAM: [====      ]  44.6% (used 13688 bytes from 30720 bytes)
```

Algo justos pero parece que tenemos juego. A ver cómo crece según vayamos añadiendo código.
