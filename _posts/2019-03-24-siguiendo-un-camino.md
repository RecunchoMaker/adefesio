---
layout: post
title:  "Siguiendo un camino"
commit: 3e8cf841d4be939d812cd43319c3ade7d2172471
---

Vamos a redefinir completamente los estados del robot, pensando en la futura implementación
de Flood Fill que se nos ha ocurrido, y teniendo en cuenta el camino guardado en el array de
celdas. Los estados que tendremos ahora serían:

- **PARADO**: es el estado inicial. No se hace nada hasta que manualmente se pasa a alguno
de los demás estados (por ejemplo, al iniciar la pexploración se pasa al estado DECIDE 
manualmente).
- **ESPERANDO**: En este estado el robot siempre ejecutará una acción de
espera de unas décimas de segundo, para "asentarlo" físicamente antes de empezar un giro o
el arranque. Después de este estado siempre se pasa al estado **DECIDE**
- **DECIDE**: en este estado el robot mira el paso marcado en la celda actual y lanza
una acción en consecuencia. Si es PASO_RECTO lanza una acción de arranque y pasa al estado
**AVANZANDO**. Si es PASO DER o PASO IZQ lanza el giro correspondiente y pasa al estado
**ESPERA**. En el caso especial de que sea el fin del camino, se lanza la acción de giro
de 180 grados, y se pasa a un estado **FIN**.
- **AVANZANDO**: también se evalua el paso de la celda actual. Si es PASO_RECTO se lanza
la acción de avance quedando en el mismo estado, y cualquier otro paso lanzará una acción
de parada. Si es un giro normal se pasará al estado esperando, y si es el fin del camino
se pasa al estado especial **FIN**.
- **FIN**: En este estado, simplemente se hace una pausa y se va al estado **PARADO**. Se añade
este estado especial para distinguirlo de los giros.

Quizá queda más claro así:

| estado/paso | PASO_RECTO        | PASO_DER        | PASO_IZQ        | PASO_STOP       |
|-------------|-------------------|-----------------|-----------------|-----------------|
| PARADO      |                   |                 |                 |                 |
| ESPERANDO   | espera,DECIDE     | espera,DECIDE   | espera,DECIDE   | espera,DECIDE   |
| DECIDE      | arranca,AVANZANDO | gira_der,ESPERANDO | gira_izq,ESPERANDO | gira_180,FIN |
| AVANZANDO   | avanza,AVANZANDO  | para,ESPERANDO     | para,ESPERANDO     | para,PARADO     |
| FIN | espera,PARADO | espera,PARADO| espera,PARADO| espera,PARADO|
  
.  
Creemos que estos estados quedan bastante claros, aunque hay que hacer alguna pequeña _trampa_ para
que funcione. Cuando se lanzan las acciones de giro, debemos evitar de alguna manera que después de
hacer la pausa lanzada en el estado ESPERANDO, cuando se vuelve a evaluar no se entre en un bucle infinito.
Podemos, o bien:

- Comprobar que la orientación del robot coincide con la orientacion del camino en ese punto y si no, actuar
en consecuencia.
- Sobreescribir el PASO en la casilla cada vez que se hace un giro, marcándolo como RECTO.

La segunda opción es la más fácil y rápida. Tiene el inconveniente que se pierde el camino original (todas
las celdas del camino quedarán marcadas con PASO_RECTO). Pero por ahora no vemos que haya problema con eso.

# Exploración

Lo que tenemos pensado en la exploración es lo siguiente: cada vez que el robot esta en estado ESPERANDO,
se hace un flood del laberinto y se marca el camino a la solución. El robot seguirá este camino, marcando
las paredes que encuentre y teniendo en cuenta que una pared frontal puede cortar el camino. Esta comprobación
se hará en DECIDE y en AVANZANDO (en DECIDE sólo hace falta para la casilla inicial). Cuando esto ocurra, se
vuelve a hacer flood y se vuelve a ejecutar el camino. Eventualmente se encontrará la solución, aunque hay
que seguir haciendo ese bucle flood/camino hasta que los pesos permanezcan inalterados en un recorrido completo
desde la posición original hasta la solución.

Al hacer el algoritmo de flood únicamente al final de las rectas, podemos saltarnos caminos mejores si
las celdas de la recta aún no fueron visitadas. Pero no creemos que eso sea un problema grave de tiempo
ya que ese camino se seguiría inmediatamente después.

# Ejecución

Una vez calculados todos los pesos de flood necesarios, se calcula el camino del origen a la solución y se
ejecutan las acciones correspondientes, aunque en este caso los giros ya no serán en redondo, si no que
se ejecutan sin detenciones tomando las curvas con un radio de 9cm.

Todo esto es muy bonito en la teoría pero veamos si somos capaces de hacer que el robot no choque con las
paredes.

# robot_siguiente_accion

Así es como ha quedado la toma de decisiones de la siguiente acción (en el commit hay además algunos prints
temporales, para depuración).

```cpp
    if (robot.estado == PARADO) {
        motores_parar();
    } else if (robot.estado == ESPERANDO) {
        accion_ejecuta(ESPERA);
        robot.estado = DECIDE;
    } else if (robot.estado == DECIDE) {
        switch (paso) {
            case PASO_RECTO: accion_ejecuta(ARRANCA);
                             robot.estado = AVANZANDO;
                             break;
            case PASO_DER:   accion_ejecuta(GIRA_DER);
                             robot.estado = ESPERANDO;
                             robot.orientacion++;
                             laberinto_set_paso(robot.casilla, PASO_RECTO);
                             break;
            case PASO_IZQ:   accion_ejecuta(GIRA_IZQ);
                             Serial.println("GIRA_IZQ");
                             robot.estado = ESPERANDO;
                             robot.orientacion--;
                             laberinto_set_paso(robot.casilla, PASO_RECTO);
                             break;
            case PASO_STOP:  accion_ejecuta(GIRA_180);
                             Serial.println("GIRA_180");
                             robot.estado = FIN;
                             robot.orientacion--;
                             robot.orientacion--;
                             break;
        }
    } else if (robot.estado == AVANZANDO) {
        Serial.println("E-AVANZANDO");
        if (paso == PASO_RECTO) {
            accion_ejecuta(AVANZA);
            robot.estado = AVANZANDO;
        } else {
            accion_ejecuta(PARA);
            robot.estado = ESPERANDO;
        }
    } else if (robot.estado == FIN) {
        accion_ejecuta(ESPERA);
        robot.estado = PARADO;
    }
```

Hemos añadido algunos comandos para definir caminos por el puerto serie y en las primeras pruebas parece que
funciona bien. Esto nos va a ser muy útil para mejorar la corrección de los sensores, ya que ahora podemos
definir caminos sin depender de las paredes.
