---
layout: post
title:  "Valor medio en leds"
commit: 41f41c0a03bba18049167056e32f5f7ca6d164c5 
---

Vamos a experimentar con el concepto de "valor medio en leds", que no es otra cosa que calcular
la media de valores entre los sensores laterales y utilizarla para el cálculo de desvío del 
centro cuando el robot se mueve en línea recta. Esto nos permitirá ajustar la dirección 
cuando tengamos al menos una pared lateral. 

# Recalibrar el valor medio

Hacemos una función que recalcule la media cuando los valores de ambos leds están relativamente
próximos

```cpp
void leds_recalibra() {
    if (leds_pared_derecha() and leds_pared_izquierda() and abs(leds_get_valor(LED_IZQ)-leds_get_valor(LED_DER)) < 10)
        leds_valor_medio = (leds_get_valor(LED_IZQ) + leds_get_valor(LED_DER)) / 2;
}
```

Y aplicamos ese término _valor medio_ cuando el robot está en estado AVANZANDO (es decir, en línea recta)

```
int16_t robot_get_desvio_centro() {
    int16_t desvio = 0;

    if (robot.estado == AVANZANDO) {
        if (pasos_recorridos + robot.casilla_offset > 200)  {
           desvio = 0;
        } else { 
           leds_recalibra();
           if (laberinto_hay_pared_derecha(robot.casilla)) desvio += leds_get_desvio_derecho();
           if (laberinto_hay_pared_izquierda(robot.casilla)) desvio += leds_get_desvio_izquierdo();
        }
    }

    return desvio;
}
```

El control de `pasos_recorridos + offset` es necesario para evitar que haya corrección cuando el robot se acerca al
final de la casilla (y los sensores laterales van a empezar a decrecer si no hay pared en la siguiente
celda). 

# Probando

El robot la mayoría de las veces se mueve aceptablemente, pero no nos acaba de convencer este sistema. A veces, si el desvío es grande
se muestra demasiado agresivo en el cambio de dirección, y muchas veces termina peligrosamente cerca de una pared.
Además, es muy poco "físico". No parece que vayamos a llegar muy lejos con esto.

# Controlar los cambios de led en función de la velocidad

Creemos que el problema de este sistema es que estamos considerando la corrección necesaria únicamente basándonos
en el valor actual de los leds. Pero los leds nos indican únicamente el desvío al centro, y no tenemos en cuenta
si nos estamos acercando o alejando, y a qué velocidad.

Lo que vamos a probar es ver si somos capaces de calcular el giro necesario para enfilar el centro, teniendo en
cuenta la _diferencia_ de valor reportada por cada led y la velocidad. En un mundo ideal con estos datos se podría
calcular el ángulo de la trayectoria respecto a la pared. Primero tendremos que ver cuánto de repetibles son las
lecturas de los led a las mismas distancias y hacer una traducción de valor de led -> distancia.
