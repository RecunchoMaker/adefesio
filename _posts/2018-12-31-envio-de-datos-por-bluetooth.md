---
layout: post
title:  "Envío de datos por bluetooth"
---

Para monitorizar el comportamiento del robot en funcionamiento es
muy conveniente disponer de una conexión inhalámbrica a través de la
cual recibir datos de depuración e incluso enviar comandos para 
lanzar instrucciones (cambios de parámetros, llamar a funciones).

Los módulos HC-05 permiten una conexión sencilla a través de bluetooth.
Estos módulos tienen 6 pines de los cuales sólo vamos a utilizar en
un principio 4 de ellos, haciendo las siguientes conexiones en 
el arduino:

| arduino| HC-05  |
|:-------|:-------|
| VCC    | VCC    |
| GND    | GND    |
| RX     | TX     |
| TX     | RX     |

Hemos puesto en nuestra baquelita un pequeño conector hembra de 6 pines
en el que enchufar el módulo, soldando por abajo las conexiones especificadas.
Así podemos enchufar/desenchufar fácilmente el módulo cuando sea necesario.

## Subida de sketch

Dado que el módulo bluetooth está conectado a las patillas RX y TX de arduino, cuando
éste esté conectado NO funcionará la subida de sketchs. Cuando queramos subir el firmware
deberemos desenchufar el módulo. Podíamos conectarlo a cualquier otra salida/entrada digital
y hacer la conexión serie por software, pero preferimos usar las entradas RX y TX 
"nativas" de arduino para poder, en el futuro, subir los sketches también de forma
inhalámbrica.

## Conexión al puerto serie desde el ordenador

Lógicamente, deberemos disponer de bluetooth en nuestro ordenador. Inicialmente
hacemos la prueba con las herramientas gráficas que tenga nuestra distribución. Emparejamos
el dispositivo y en nuestro caso, aparece un puerto /dev/rfcommX (siendo X un número secuencial, creándose
un puerto nuevo cada vez que establecemos una nueva conexión).

Una vez creado este puerto en el emparejamiento, simplemente debemos especificar
en el comando de depuración del puerto serie el puerto al que nos queremos conectar.

```
pio device monitor --port /dev/rfcomm0
```

## Enlaces
(Conetar arduino a bluetooth)[https://www.instructables.com/id/Arduino-AND-Bluetooth-HC-05-Connecting-easily/]

