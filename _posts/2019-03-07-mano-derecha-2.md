---
layout: post
title:  "Mano derecha (2 de...)"
---

Vamos a desinflar un poco las pretensiones y utilizar únicamente giros en redondo a la
hora de explorar. Postermiormente, cuando recorramos el laberinto con el camino aprendido,
lidiaremos con los giros y la sincronización que pueden aportar los sensores.

# Un montón de estados

Hemos metido un montón de estados por los que va pasando el robot a la hora de explorar. En
secuencia serían:

- PARADO
- EXPLORANDO INICIAL
- EXPLORANDO
- PAUSA PREGIRO
- PARADO PARA GIRO
- PAUSA INICIAL

y de PAUSA INICIAL volveríamos a EXPLORANDO. 

De esta manera intentamos controlar la secuencia de acciones que se van ejecutando, aunque
claramente no es una forma buena. Pero volvemos a repetir que queremos ver el robot moviéndose
y experimentar.

# Fallos en la detección

Creemos que casi tenemos lista la detección de paredes en todas las direcciones, pero a veces
ocurre que no se consigue detectar la pared frontal y el robot intenta ejecutar un segmento
recto de 18cm. Acaba parando bruscamente en un control _ad hoc_ que introducimos, pero no
controla bien el cambio de casilla y se acaba mapeando mal el laberinto.

commit:

# Montones de Serial.print en las rutinas de interrupción

Tenemos una cantidad de exagerada de Serial.print en las interrupciones para evaluar lo que
ocurre... Quizá incluso esten afectando al cálculo de velocidades... Una vez tengamos el mapeo
listo, decididamente hay que empezar a limpiar sin falta.

commit: cfe346171b2cb92892ae8fca5e030988fa3281b2

