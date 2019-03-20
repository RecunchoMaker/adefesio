---
layout: post
title:  "...y posible solucion"
commit: 632fa09df7b97ea50d0e0289560788bdb1714045
---

Hemos hecho mil pruebas. Además de esa disminución gradual de los valores, a veces, despues de hacer
un montón de metros correctamente, de repente los valores de los sensores bajan (y todo empieza a fallar).
Hemos probado un montón de cosas en software, creyendo que podríamos dar con algún problema ahí.

Pero finalmente hemos visto algo que nos hace pensar que sea un problema de conexiones

# Esperando valores correctos de log

Tal y como dejamos el commit de hoy, el robot no arranca si no tiene los sensores laterales
por encima de 200. Cuando estaba en ese estado, simplemente reseteabamos (a veces un par
de veces) y todo iba como la seda (hasta que volvía a haber una caída de lecturas de forma aleatoria).

Pensábamos que el reset, de alguna manera arreglaba el problema. Nuestro chasis no es completamente
rígido, sino que la placa posterior se mueve un poco con la presión. Cuando hacíamos reset, no era
el proceso del micro el que hacía que volviera a funcionar. Hemos comprobado que simplemente haciendo
presión en la placa también se corrige.

Así que tocara desmontar a Adefesio, comprobar conexiónes y retocar las soldaduras que veamos problemáticas.


