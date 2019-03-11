---
layout: post
title:  "Casilla offset"
commit: 4b82f97acadb01a48a986ed972426dcb53c50ca0
---
Antes de encontrarnos el problema de los sensores habíamos introducido unas
cuantas variables en `robot.cpp`. 

```cpp
typedef struct {
    tipo_estado estado:2;
    tipo_orientacion orientacion: 2;
    uint8_t casilla = 0;
    float casilla_offset = 0;
} tipo_robot;
```

Lo que trataremos es mantener en una variable de ese tipo la orientación y
la posición del robot, así como su estado (por ahora únicamente _parado_ o
_explorando_.

También eliminamos el array de acciones. Ahora, sólo hay una variable de _tipo accion_, y
cuando una acción se termina, se crea la siguiente. En una primera prueba simplemente
vamos acumulando en _casilla offset_ la distancia recorrida desde el origen de la primera
casilla. Aún falta mucho por programar (teniendo en cuenta orientaciones, curvas, etc). Lo
único que hace el commit de hoy es medir esta distancia desde el arranque del robot,
teniendo encuenta que se empieza desde el centro de una casilla (el offset es igual a 8 cm)
y únicamente recorremos un pasillo con paredes a ambos lados.
