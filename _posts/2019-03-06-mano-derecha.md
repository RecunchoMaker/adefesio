---
layout: post
title:  "Mano derecha (1 de...)"
commit: fd0263fc352d23b887f37d6bf5725ccd0c51d88b
---

Ayer no andábamos desencaminados pero se nos habían olvidado un par de detalles: cuando nos encontramos
un callejón sin salida tenemos que recorrer media casilla, girar en redondo, y recorrer de nuevo la
media casilla de vuelta. También teníamos algun bug en el tratamiento del offset.

Realmente el algoritmo de seguir los huecos a la derecha es una excusa para probar el mapeado de casillas
y comprobar que se están interpretando bien las orientaciones y las paredes, y tener información suficiente
para estudiar el comportamento de los sensores siguiendo un camino real. En el futuro utilizaremos un
_flood fill_, que es capaz de explorar más eficientemente (se saltaría zonas de laberinto que no
compensa explorar) y además calcularía el camino más corto (si hablamos de _distancias Manhattan_).

# Sincronización con paredes

Como era de esperar, sin tener en cuenta los sensores nada más que para enderezarnos en los pasillos,
el robot acaba chocando contra alguna pared. Introducimos un pequeño control que actualize los pasos
objetivo cuando estamos en una casilla que tiene una pared lateral pero el sensor nos devuelve que
ya no está: en este momento estamos a la distancia sensor/rueda del final de la casilla.

# CASI se recorre el laberinto

Queríamos conseguir hoy recorrer todo nuestro pequeño laberinto de 5x5 pero nos hemos quedado un poco a medias.
Se atasca en algún callejón sin salida (no controlamos la distancia en la que se queda de las paredes), 
los giros en U son un poco erráticos y en general hay mucho por cambiar.

# Código muy confuso

El código está quedando muy confuso. Cuando seamos capaces de recorrer el laberinto completamente
y mapear las paredes, habrá que refactorizar un montón de código y extraer lo importante. Particularmente,
no nos acaba de convencer tener separado el avance de casilla de la acción... habrá que repensar todo esto.
Pero primero queremos tener el robot recorriendo correctamente todo el laberinto (en exploración) y localizar
lo necesaro para que funcione bien.
