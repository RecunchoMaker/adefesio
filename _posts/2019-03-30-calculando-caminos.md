---
layout: post
title:  "Calculando caminos"
commit: 0bb927d55053e566e14560a969bce9b771d7342c
---

Ahora que ya tenemos las rectas "aproximadamente" controladas, vamos a
programar los caminos que nos dan los pesos de flood.

Para ello simplemente escogemos el mejor vecino de la casilla en la que estamos y
vamos marcando el camino en el array _laberinto_.

```cpp
void camino_recalcula() {
    camino_init(robot_get_casilla(), robot_get_orientacion());
    while (camino_casilla_actual != CASILLA_SOLUCION) {
        siguiente = flood_mejor_vecino_desde(camino_casilla_actual);

        for (dir = 0; dir < 4; dir++) {
            if ((int) siguiente - camino_casilla_actual == incremento[dir]) {
                switch ((camino_orientacion_actual - dir)) {
                    case 0:  camino_anadir_paso(PASO_RECTO); 
                             break;
                    case 1:  camino_anadir_paso(PASO_IZQ);
                             break;
                    case -1: 
                    case  3: camino_anadir_paso(PASO_DER);
                             break;
                }
            }
        }
    }

}
```

# Bucle flood/camino

Hemos introducido unos cuantos estados más en el robot:

- FLOOD: En este estado el robot hace una iteracción corigiendo los pesos de
flood en todo el array. Si se ha hecho algún cambio, se continúa en este estado.
Si no, se va al estado REORIENTA
- REORIENTA: Añadimos este estado para reorientar el robot cuando, después de
terminar un camino (estamos en una casilla con PASO_STOP) el nuevo camino encontrado
comienza con otra orientación. En otras palabras: debemos dar la vuelta 180 grados

Hay que añadir un _if_ en el estado AVANZANDO, ya que ahora vamos a seguir un camino
que puede interrumpirse en cualquier momento si encontramos una pared frontal. En
este caso, marcamos la casilla como PASO_STOP y volvemos al estado DECIDE.


# Primeras pruebas

Falta toda la lógica que ocurre cuando se encuentra la solución (y hay que volver al
origen, repitiendo el proceso hasta que no hay caminos por descubrir). Pero queremos
ver cómo se comporta Adefesio corrigiendo únicamente las trayectorias en las rectas.

Tal y como ha quedado el commit hoy, el robot llega a la solución aunque en nuestro
laberinto pequeño con muchos giros, éstos no están bien sincronizados lo que hace que
se vaya desfasando cada vez más, y en varios casos acaba chocando con una pared.

También tenemos algún bug en los giros: a veces el robot gira de más y luego retrocede...
hay que repasar la máquina de estados.

Por el momento parece que vamos bien, pero tenemos que tratar urgentemente estos temas:

- Corregir los giros apoyándose en la información de los sensores
- Corregir la frenada para que quede a la distancia correcta de la pared frontal (si la
hubiera)
