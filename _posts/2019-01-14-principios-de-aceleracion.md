---
layout: post
title:  "Principios de aceleracion"
---

Queremos introducir un pequeño control de la aceleración, debido a que en las pruebas
que hacemos el robot hace un "caballito" al pasar rápidamente a una velocidad (aunque no sea muy alta). Seguimos
con la intención de hacerlo en medidas internacionales, así que introduciremos la constante
`MAX_ACELERACION` en nuestro `config.h`

No tenemos muy claro cómo será el control a _alto nivel_ del robot, así que no nos
detendremos mucho: el objetivo principal es evitar ese caballito al arranque. Así que
únicamente introducimos una función `motores_set_velocidad`, que actualiza una
variable que contiene la velocidad objetivo. Además, en nuestro timer sustuimos el `actualiza_pwm`
por el `actualiza_velocidad` que incrementará la velocidad actual teniendo en cuenta el periodo
del timer.

# Pruebas de distintos valores 

Hemos encontrado que una aceleración de 0.5 m/s2 es bastante aceptable y hace que el robot
alcance la velocidad máxima rápidamente y muy suavemente. Estamos utilizando la aceleración
como una medida de incremento de la velocidad deseada, y no de la velocidad real: Esto no
es estrictamente correcto pero parece dar buenos resultados. Lo anotamos para posibles
mejoras.

# Otro disclaimer

El commit de hoy está algo sucio también porque no hemos tenido mucho tiempo y aún no
está claro si estas funciones son adecuadas para un control de aceleración.

commit: 8cf83b65b675a3cad75723b1cd0dd5cfb14dc326
