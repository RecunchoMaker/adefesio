---
layout: post
title:  "Mano derecha 3 de 3 y mapeando correctamente"
commit: 77bad88f9e858bf9f55a821b4836d33e1e458f52
---

Ayer nos faltaban un par de pequeños detalles: no estábamos mapeando
las paredes frontales, y "a veces" el robot se colgaba y empezaba a destrozar
las paredes sin ningún sentido. Lo primero fue fácil pero lo segundo creemos
que es porque no teníamos el indicador `static` en algunas variables y pudiera
ser que se estuvieran machacando valores de alguna manera. Tampoco lo tenemos
demasiado claro.

El caso es que ahora el robot se mueve correctamente por el laberinto, mapea
todas las paredes y nos saca un montón de información sobre lo que está haciendo.
Por ejemplo:


```
33 	leds:0	42	28	183	0.20 	2 	0.0000 	9089 	3.553192 	0.20 	-2.000
chocamos! leds:
268/333, pasos objetivo =236 recorridos =221
pausa pregiro
* accion: dist:0.00 acel= 0.00 vmax=0.00 vfin=0.00 deceleracion=0.00 pasos_hasta_dec=-2147483648 pasos_obj=0 r=0.20
leds:1	333	268	160	
giro izquierda
* accion: dist:0.05 acel= 1.00 vmax=0.20 vfin=0.10 deceleracion=1.00 pasos_hasta_dec=94 pasos_obj=133 r=0.00
pausa inicial
* accion: dist:0.00 acel= 0.00 vmax=0.00 vfin=0.00 deceleracion=0.00 pasos_hasta_dec=-2147483648 pasos_obj=0 r=0.20
Orientacion 2
+-----+-----+-----+-----+
|   0     1 |   2     3 | 
|           |   V       |     
+     +     +     +-----+
|   5     6 |   7     8 | 
|           |           |     
+     +-----+-----+     +
|  10    11    12    13 | 
|                       |     
+     +-----+     +     +
|  15 |  16 |  17 |  18 | 
|     |     |     |     |     
+     +     +     +-----+
|  20 |  21    22    23 | 
|     |                 |     
+-----+-----+-----+-----+

* accion: dist:0.09 acel= 1.00 vmax=0.20 vfin=0.20 deceleracion=1.00 pasos_hasta_dec=236 pasos_obj=236 r=99999.00
leds:0	15	24	106	
cambio a casilla 7
Orientacion 2
+-----+-----+-----+-----+
|   0     1 |   2     3 | 
|           |           |     
+     +     +     +-----+
|   5     6 |   7     8 | 
|           |   V       |     
+     +-----+-----+     +
|  10    11    12    13 | 
|                       |     
+     +-----+     +     +
|  15 |  16 |  17 |  18 | 
|     |     |     |     |     
+     +     +     +-----+
|  20 |  21    22    23 | 
|     |                 |     
+-----+-----+-----+-----+
```

# Problemas de memoria

Empezamos a estar al límite de la memoria disponible.

```cpp
Memory Usage -> http://bit.ly/pio-memory-usage
DATA:    [========= ]  89.5% (used 1832 bytes from 2048 bytes)
PROGRAM: [======    ]  59.9% (used 18388 bytes from 30720 bytes)
```

Cuando empezemos a limpiar seguramente conseguiremos liberar algo pero nos queda muy poco. En el array que guarda
los datos del laberinto ya tenemos "reservados" un par de variables ahora mismo sin uso (quizá una para los valores
de la inundación y otra para la cola de proceso del algoritmo), y siempre creamos el array de 17x17 (aunque estemos
probando en 5x4) así que parece que llegará, pero hay que tener cuidado.


# Siguientes mejoras

Ahora que ya tenemos las pruebas de concepto listas, y vemos que más o menos funcionan, intentaremos limpiar y
documentar el código decentemente. Hay muchas cosas que implementar pero creemos que merecerá la pena dejar
el código legible antes de empezar. Entre estas cosas estarían:

- Obtención de distancias en función del valor de los leds.
- Aumentar la distancia de detención de los leds frontales (menos resistencia en la placa?)
- Corrección contínua en giros función del patrón de las paredes cercanas
- Detección de _hitos_ (como la caída de valor de un sensor lateral en recta que indica que estamos a 4cm del final de la casilla

Y por supuesto, implementar flood fill para acelerar la exploración. Por ejemplo, en el laberinto que tenemos
ahora mismo hay una pared entre 5 y 6 (que aún no se había detectado en el ejemplo de arriba). Con flood fill,
el robot exploraría en la secuencia 20-15-10-5-0-1-6 y en ese momento ya tendría toda la información para ir
directo a la llegada, ya que nunca bajaría de 12 a 17 ni de 13 a 18, dado que habría casillas más cercanas a las
que ir. Tenemos muchas ganas de implementar esto para notar cierta "inteligencia" en el movimiento. Pero lo
primero será lo primero.
