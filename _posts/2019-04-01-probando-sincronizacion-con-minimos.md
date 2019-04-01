---
layout: post
title:  "Probando sincronización con mínimos"
commit: 
---

Estuvimos probando a sincronizar el robot entre 4 viguetas sueltas. El robot en
el centro, haciendo giros de 90 grados sobre si mismo. Lo que
hacemos es: calculamos la media de pasos a la que los sensores frontales han detectado
la mínima distancia. Esta media se correspondería teóricamente con el robot mirando
exactamente a la vigueta. Luego añadimos o disminuimos los pasos objetivo del giro
en función de la diferencia de esa media con la que consideramos ideal.

La prueba ha resultado bien. El robot se mantiene girando correctamente de forma indefinida
e incluso moviendo todas las viguetas vuelve a cuadrarse en la casilla. Hay un fallo grave
en este cálculo y es que no estamos considerando la distancia. Pero como prueba nos ha
valido.

Lanzamos el robot por un laberinto real y ha chocado al poco rato. Otro fallo importante:
esta sincronización con viguetas no se puede hacer si tenemos una pared enfrente antes del
giro, por razones obvias. Hay que considerar este caso de forma particular.

Para el caso de que exista una pared frontal, tendremos que tener en cuenta la mínima distancia
del sensor del lado del giro. Además, ese cálculo es independiente del resto de paredes: siempre
nos podemos fiar de ese sensor.
