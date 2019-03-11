---
layout: post
title:  "Controlando la velocidad - caso 3. Y conclusiones"
commit: 62d9e7bedb7475f1698fa45ca61228fe00c52347
---

El caso 3 se produce cuando hay varios ciclos de timer sin producirse
ninguna actualización del encoder. Entonces, más que un cálculo real de la velocidad
de la que no tenemos información actualizada, habrá que estimarla basándose información anterior.

Veamos un caso típico, en el que el encoder se dispara cada 2 ciclos de timer:

```
      +---+     +---+     +---+     +---+     +---+     +---+     +---+
      |   |     |   |     |   |     |   |     |   |     |   |     |   |
      | 1 |     | 2 |     | 3 |     | 4 |     | 5 |     | 6 |     | 7 |
    --+   +-----+   +-----+   +-----+   +-----+   +-----+   +-----+   +----
Caso 3:       1                 2                     3     

```
En el milisegundo 3, 5 y 7 se está produciendo el caso 3. Vamos a coger como
ejemplo de cálculo lo que haremos en el milisegundo 5. La velocidad calculada
en el ciclo anterior (en la ejecución del timer correspondiente al ms 4) es la 
última que tenemos de refeferencia. El el ms 5 no se ha producido ningún salto
de encoder, así que lo único que sabemos con certeza es que el número de ticks
hasta el futuro salto va a ser mayor que los ticks desde el salto 2 hasta el
ms 4. 

# Aproximción 'optimista'

Una solución posible sería devolver siempre la mayor velocidad posible (o
lo que es lo mismo, el mímimo número de ticks posibles), que se correspondería
con el número de ticks sin actualizar * OCR1A mas la diferencia entre OCR1A y
el ultimo tcnt1 capturado.

Este esquema es sencillo y rápido de de ejecutar, aunque
le vemos el inconveniente en situaciones de equilibrio (velocidad constante):
la velocidad real va a ir alternando entre dos valores de los cuales sólo 1
es el correcto, lo que a priori parece que va a dificultar la convergencia del PID.

# Aproximación 'conservadora'

Otra aproximación consistiría en devolver el último valor de velocidad calculado,
a menos que claramente la velocidad tenga que ser inferior. Vamos a intentar
dibujar un esquema en el que las rayas verticales se corresponden con los
saltos del timer, y los números de la parte inferior se corresponden con
los disparos del encoder.

```
      +   +   +   +   +   +   +   +   +   +   +   +   +   +   +   +   +
      |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
      1   2   3   4   5   6   7   8   9   0   1   2   3   4   5   6   7
    --+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
        1       2           3               4       5       6
```

En el ms 4 (caso 2) se calcula el número de ticks que transcurren desde el salto 1 hasta el 2. Por simplificar, suponemos que todos los saltos
se hacen en el punto medio de un OCR1A de 10. Los ticks calculados en el ms 4 serían 20. En el ms 5, tenemos el caso 3. Los ticks mínimos
serían 10 + (10-5) = 15. Como no superan los 20 anteriores, no modificamos la velocidad calculada y devolvemos los mismos 20.
En el ms 6, sin embargo, los ticks mínimos aumentan 10, quedando en 10 + 10 + (10-5) = 25. Claramente la velocidad está disminuyendo, así
que, aunque no sabemos cuánto, devolvemos 25 como nueva velicidad.

En el siguiente cálculo (ms 7) ya se ha producido un salto de encoder y hacemos un cálculo con el caso 2.

# Prueba 'conservadora'

Nos gusta más la segunda opción, que tampoco parece demasiado complicado de programar. Hacemos una prueba
y estos son algunos de los resultados en un pwm intermedio:

```
pwm = 160
caso:3   sin act:0   encoder left: 0   ticks: 28727   0,1 = 9102 6111 
caso:2   sin act:0   encoder left: 1   ticks: 26602   0,1 = 788 11391 
caso:3   sin act:0   encoder left: 0   ticks: 26978   0,1 = 10497 5421 
caso:2   sin act:1   encoder left: 1   ticks: 26874   0,1 = 9012 3888 
caso:2   sin act:1   encoder left: 1   ticks: 26649   0,1 = 7012 1663 
pwm = 170
caso:2   sin act:1   encoder left: 1   ticks: 26749   0,1 = 14822 9573 
caso:3   sin act:0   encoder left: 0   ticks: 24896   0,1 = 8220 1269 
caso:3   sin act:0   encoder left: 0   ticks: 25678   0,1 = 6320 15129 
caso:2   sin act:1   encoder left: 1   ticks: 25062   0,1 = 13887 6951 
caso:2   sin act:1   encoder left: 1   ticks: 24809   0,1 = 11160 3971 
pwm = 180
caso:2   sin act:1   encoder left: 1   ticks: 25047   0,1 = 14986 8035 
caso:3   sin act:0   encoder left: 0   ticks: 29331   0,1 = 10000 1438 
caso:2   sin act:0   encoder left: 1   ticks: 23414   0,1 = 3677 11092 
caso:2   sin act:0   encoder left: 1   ticks: 23510   0,1 = 2641 10152 
caso:2   sin act:1   encoder left: 1   ticks: 23354   0,1 = 15498 6854 
```

Por supuesto hay diferencias entre el caso 2 y el caso 3, pero tampoco es mucha. Vamos a dejar los
cálculos así y continuamos avanzando con más cosas.


## Conclusiones

Hay varias cosas que no nos gustan del cálculo de velocidad que hemos hecho:

- Hemos tenido que reducir a la mitad la resolución del encoder
- A velocidades bajas, la mayoría de las veces debemos de estimar el resultado (caso 3)

Seguramente tengamos que retomar la programación de estos cálculos.
El problema que vemos es que para calcular la posición del robot en 2D deberíamos actualizar
las velocidades muchas veces por segundo. El espacio es la integral de la velocidad, y a
mayor frecuencia de muestreo (diferencial de tiempo más pequeño) reducimos el error acumulado.
Dado que las dos ruedas inevitablemente se van a mover a distintas velocidades, la precisión en el control
de la trayectoria (para cuyo cálculo necesitamos los espacios recorridos) depente directamente de esta frecuencia.
De todos modos, vamos a continuar y en los próximos días veremos si esto ha valido para algo.

Siempre podemos volver a utilizar el disparo de la interrupción en modo CHANGE si tratamos
de forma distinta el flanco de subida y de bajada para tener en cuenta la diferencia entre
ambos saltos debido a las propias características físicas del encoder.

Al igual que hicimos ayer, también vamos a dejar en este commit código comentado de prueba para
dejar constancia, pero para los siguientes eliminaremos todas esas variables `last_` del fichero `encoder.cpp`,
duplicaremos el código para el encoder derecho, y dejaremos todo más limpio.
