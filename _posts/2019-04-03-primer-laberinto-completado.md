---
layout: post
title:  "Primer laberinto completado"
commit: 9c6c9dee0fc46ebb89229ae2b804a2cea58494ed
---

Seguimos con el laberinto de ayer. Hemos añadido las constantes _KP PASILLO1_ y
_KP PASILLO2_ con sus respectivos comandos, correspondientes a la proporción en la
que el robot se intenta acercar a la línea central: uno proporcional al desplazamiento
lateral y otro proporcional al incremento de ese desplazamiento.

En estas constantes, eliminamos todos los cálculos referentes a la distancia de convergencia,
longitudes de robot... etc. Sólo dejamos las variables que interesan porque nos estábamos
haciendo un lío con los cálculos. El robot ahora va bastante recto si empieza ya centrado. Si
no, hay un ligero cabeceo que no conseguimos eliminar del todo, pero por ahora quedará así.

Modificamos la creacción del camino, después de hacer un flood, para que en la primera casilla
que encuentre no visitada continúe recto.

Cuando el laberinto tiene muchas referencias (es decir, muchas paredes y pocas o ninguna isla,
parece que el robot se comportará bien únicamente con esta corrección de dirección y la frenada ajustada 
delante de una pared frontal. Iremos complicando el laberinto eliminado paredes, y añadiendo
las correcciones que habíamos pensado (y seguramente alguna más) pero ahora
nos interesa más hacer el ciclo completo:

1. Ir haciendo flood y camino hasta llegar a la solución
2. Hacer un nuevo flood desde la casilla solución hasta la casilla original
3. Repetir 1 y 2 hasta que no se hayan visitado nuevas casillas
4. Esperar la señal de inicio de solución (poner y sacar la mano delante de los sensores frontales)
5. Ir a la solución y volver.
