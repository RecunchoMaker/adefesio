---
layout: post
title:  "Cambio en logs"
commit: 8fdc65a73986d8766e81d2ed2d2d93bc680b068c
---

Formateamos para Doxygen otros ficheros más (`leds.cpp` y `led.h`) y continuamos con `log.cpp`.
Cuando empezamos a programar, habíamos configurado distintos entornos de compilación
en `platform.ini`, que definían distintos flags para compilar funciones que logeaban
uno u otro conjunto
de variables. Incluso también habíamos definido un array en el que guardar
datos en cada interrupción de reloj, para poder obtener una especie de log en tiempo
real de un intervalo de tiempo (que tenía que ser pequeño por la limitación de memoria).

Con el tiempo vemos que este esquema no es muy práctico y hace semanas que no lo estamos
utilizando. Así que vamos a eliminar todos estos entornos, y simplificar `log.h` y `log.cpp`.
Dejamos únicamente tres funciones, que loguean distintas variables, y ya irán surgiendo las
nuevas necesidades.

```cpp
void log_leds();
void log_accion();
void log_motores();
```

# Macro F

En el arduino tenemos mucha más memoria Flash (en la que se almacena el programa) que SRAM
(donde se almacenan las variables). Vamos a intentar guardar en Flash todos los strings que utilizamos
para almacenar los literales que se envían al puerto serie, para intentar liberar memoria
SRAM que nos va a hacer falta para la implementación de flood fill. Utilizamos para eso la macro `F()`.

[Arduino memory](https://playground.arduino.cc/learning/memory)

Por ejemplo:

```cpp
Serial.print(F("* accion: dist:"));
```

Antes, el espacio ocupado en SRAM era de 1533 bytes: 
```
Linking .pioenvs/nanoatmega328/firmware.elf
Checking size .pioenvs/nanoatmega328/firmware.elf
Memory Usage -> http://bit.ly/pio-memory-usage
DATA:    [========  ]  75.8% (used 1553 bytes from 2048 bytes)
PROGRAM: [======    ]  57.9% (used 17790 bytes from 30720 bytes)
```

Después de utilizar la macro `F()` en el archivo `log.cpp`:
```
Linking .pioenvs/nanoatmega328/firmware.elf
Checking size .pioenvs/nanoatmega328/firmware.elf
Building .pioenvs/nanoatmega328/firmware.hex
Memory Usage -> http://bit.ly/pio-memory-usage
DATA:    [=======   ]  71.6% (used 1467 bytes from 2048 bytes)
PROGRAM: [======    ]  58.1% (used 17850 bytes from 30720 bytes)
```


