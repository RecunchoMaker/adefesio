---
layout: post
title:  "Detectando casilla"
commit: aafd867de4a276ddb981ad4b8a0f251e59ea9108
---

Ahora vamos a intentar saber en que casilla estamos, acumulando los offset al
terminar cada acción y, si el offset es mayor que el tamaño de la celda, es que nos hemos movido
de casilla y podemos muestrear las paredes laterales y apuntarlas.

```
+-----+-----+-----+-----+
|   0 |   1     2     3 |
|   A |                 |
+     +     +     +     +
|   5 |   6     7     8 |
|     |                 |
+     +     +     +     +
|  10    11    12    13 |
|                       |
+     +     +     +     +
|  15 |  16    17    18 |
|     |                 |
+     +     +     +     +
|  20    21    22    23 |
|                       |
+-----+-----+-----+-----+
```

Esto es lo que nos sale en una primera prueba, en la que el robot va recto de 20 a 0.
La _A_ marca la posicion y orientación final del robot.

- La casilla 20 tiene una pared al oeste pero no se marca. Es un caso particular (al
inicio) que hemos considerado pero no sabemos porqué no sale.
- Las demás casillas se detectan bien
- El robot para bruscamente en la casilla 0, parece que no decelera correctamente.
- Nos parece que se está perdiendo la cuenta de las distancias en algún bug, porque
da la impresión que se avanza de casilla algo tarde. Tendremos que depurar y confirmar
que estamos haciendo bien el cálculo de distancias, pero a priori parece que no.

Hemos puesto unos cuantas funciones en `leds.cpp`, para calcular las distancias a
la pared frontal, y también devolver la variación entre los leds laterales cuando nos falta
alguna pared de referencia para usar en el pid de `motores.cpp`. Todo esto se hizo rápido para hacer esta prueba pero hay
que reescribirlo.
