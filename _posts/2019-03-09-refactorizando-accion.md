---
layout: post
title:  "Refactorizando acción"
commit: 99d484c4cdaeb0793ee75f858f011ad18ab6223f
---

El fichero `robot.cpp` se está volviendo un poco grande y tiene demasiadas
responsabilidades. Vamos a separar en otro fichero el tratamiento de las acciones,
de manera que `robot.cpp` simplemente sabe que tiene que ir recto, o girar, pero
no sabe cómo hacerlo. De eso se encarga el nuevo fichero `accion.cpp`.

Además en este hay varias decisiones que se toman en función
de valores de sensores, velocidades... y queda muy confuso saber realmente por qué
hace esta u otra cosa. Por ejemplo:

```cpp
if (accion.radio == INFINITO and accion.velocidad_final > ROBOT_V0 ) {
```

Lo sustituimos por una llamada a la función:
```cpp
bool accion_cambio_casilla() {
    // TODO: valido sólo en exploración
    // Si vamos rectos y no vamos a frenar -> la acción implica un cambio de casilla
    return (accion_radio == RADIO_INFINITO and velocidad_final > ACCION_V0 );
}
```

# A veces no se mapean bien las paredes

Vamos haciendo esta refactorización poco a poco: en cada cambio volvemos a subir el
sketch y hacemos la prueba en el laberinto. A veces, sólo a veces, no se mapean bien las paredes,
así que debe haber algún error oculto que la mayoría del tiempo funciona por simple
casualidad. Pero seguimos moviendo las cosas, esperando que este error aparezca al
ir aclarando el código.

Como referencia, creamos una carpeta `logs` en la que dejamos una traza completa de
un mapeo correcto

# Mover control de velocidades

Movemos también todo el control de velocidades a `accion.cpp`. Así en `robot.cpp` sólo nos
ocupamos de la lógica a "alto nivel". En el futuro, `accion.cpp` decidirá las velocidades
a la que tendremos que tomar las rectas y las curvas en función de si estamos explorando
nuevas casillas o o no. Quizá incluso haya que separar las velocidades en su propio fichero.

Por hoy lo dejamos aquí. Seguimos teniendo que quitar varias cosas de `robot.cpp`, pero
empieza a aparecer un poco más claro.
