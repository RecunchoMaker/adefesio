---
layout: post
title:  "Introduciendo velocidad angular"
commit: 04f5f0b3a8327ee1fc02459458359e8d5338479b
---

Vamos a intentar introducir un cálculo de la velocidad angular para que el robot
corrija los desvíos. Una manera sería contar directamente los pasos de encoder 
y compensar las velocidades de ambos motores, aunque no nos parece muy "serio". Además,
la intuición nos dice que en el futuro, cuando tengamos que dirigir el robot por el
laberinto, nos harán falta estos cálculos para poder suavizar los giros en la medida
de lo posible y no limitarse a parar y girar sobre sí mismo 90 gradas.

# Radio y ángulo de curvatura

Dado el avance _e1_ y _e2_, de ambas ruedas, definimos el radio de curvatura como el
radio del círculo que contiene la trayectorio del punto medio entre ambas ruedas, siendo
_d_ la distancia del punto medio a cada una de ellas (es decir, la distancia entre las 
ruedas es _2d_. A partir del radio, utilizamos cualquiera de las ecuaciones de _e1_ o _e2_
para calcular el ángulo de giro. 

Quizá algún día (lejano) pase esto a ecuaciones en markdown, pero por ahora lo ponemos así.

![velocidad_angular](  ../assets/2019-01-11-radio.jpg)

Si los especios de ambas ruedas son iguales, lógicamente no hay velocidad angular y no 
existe el radio. Además, si el radio del movimiento es igual a la _d_, es que una de las
ruedas está parada. Para evitar la indeterminación tomaremos en la segunda fórmula el valor
distinto de 0.

# Fichero _settings.h_

Creamos un nuevo fichero `settings.h`, que por ahora sólo contiene la distancia entre ejes
de nuestro robot. No hemos querido ponerlo en `encoders.cpp` por que no es una característica
propia del encoder. Quizá en un futuro agrupemos en este fichero todas las constantes de 
configuración (parámetros de PID, pasos por vuelta del encoder... etc) que ahora mismo
están en los ficheros relacionados.

# Algunas horas más tarde...

Lo vamos a dejar por hoy porque estamos algo embotados. El parámetro Kp para la velocidad
lineal no parece activarse hasta que es bastante grande, momento en el cual todo se desmadra.
No hizo falta muchos días para caer en la cuenta de que es muy conveniente utilizar unidades
estándard (metros, segundos... etc), como decían los creadores de Bulebule. Por mucho que
logeemos variables, no tenemos una referencia de lo que significan. Por aquello de ahorrar
operaciones de coma flotante estamos utilizando ticks como medida inversa de la velocidad,
el arco de un paso de encoder como medida de distancia... así es imposible (o muy difícil
y, desde luego, nada claro).

Así que los siguientes días vamos a dedicarnos a limpiar un poco el código, y definir
constantes de conversión para hacer los cálculos en las unidades correctas.

El commit de hoy no aporta nada excepto confusión.
