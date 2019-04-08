---
layout: post
title:  "Fin de la primera parte"
commit:
---

Estos últimos días han sido un poco caóticos porque queríamos tener algo
listo para enseñárselo a nuestros amigos en el Arduino Day de Lugo. Al final,
después de un último ajuste _in situ_, conseguimos que funcionara "más o menos",
lo suficiente para dar una idea de lo que va a ser.

Nos falló inicialmente por dos cosas: había bastante luz natural (que nos daba
de lleno en una ventana) y vimos que la lectura con el diodo apagado era algo mayor
a la que obtenemos en nuestro zulo. Hasta aquí todo bien, porque nosotros restábamos
esa lectura a la obtenida con el led encendido. Pero la curva de respuesta de los diodos en función de la
distancia varía, y nuestro control de pared frontal no tenía eso en cuenta. Vamos a
tener que hacer calibraciones distintas de los diodos en otras condiciones y ver si
se puede calcular a partir de una lectura "en apagado", o buscar un sistema de calibración
rápido. Los diodos frontales parecen fáciles (tenemos encoders y podemos mover el coche
a mano desde una distancia conocida), pero los laterales parece algo más difícil. Ya
se nos ocurrirá algo.

El sistema para corregir el ángulo de giro cuando tenemos de referencia una pared frontal
tampoco funciona lo bien que debiera. Aún hay que darle más vueltas.

Ahora toca dedicarle unos días a ordenar todos estos conceptos y limpiar el código (y nuestro
taller) Vamos a dejar de hacer commit por cada post y empezaremos a subir cambios por temáticas,
como los programas normales.
Seguramente escribamos varios posts más, pero no serán frecuentes a menos que haya varias cosas
que comentar o cambios importantes.
