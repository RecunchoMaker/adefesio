---
layout: post
title:  "Posición del robot"
---

Queremos empezar a combinar los movimentos de giro y rectos y afinar
los parámetros del pid (quiza incluso cambiar el algoritmo). Para hcerlo
de forma conjunta vamos a hacer unas pruebas que muevan el robot una
secuencia de movimientos (simplemente 40 cm hacia adelante y girar sobre
sí mismo). Haremos que se ejecuten estos movimientos varias veces y veremos
la repitibilidad.

# Ficheros robot.h y robot.cpp

Creamos un nuevo fichero en el que incluiremos unas funciones de control
a alto nivel. Aún no sabemos qué funciones necesitaremos realmente en el
futuro, pero lo importante ahora es estudiar varios movimientos y cómo le
afectan los parámetros. Creamos unas cuantas funciones autoexplicativas:

```cpp
void robot_set_posicion(float x, float y);
void robot_set_orientacion(int8_t o);
float robot_get_posicion_x();
float robot_get_posicion_y();
int8_t robot_get_orientacion();
void robot_actualiza_posicion();
void robot_ir_a(float x, float y, float radio);
void robot_gira(int8_t orientacion);
void robot_parar();
```

La idea es poder poner en el main cosas como `robot_set_posicion(0.0, 0.018)`.
La función comprobará la orientación correcta (que es una constante entera, para
cada punto cardinal) e incrementará/disminuirá las coordenadas. Por ahora, este
incremento se hará suponiendo que hace una recta perfecta en la orientación
actual (no queremos introducir senos y cosenos en un cálculo de interrupción).

# Cambios varios

Hemos hecho varios cambios en el código, incluyendo el traslado de las funciones
de log a su propio fichero. Lo hicimos asi porque creamos una nueva función para
logear datos de este fichero `robot.cpp`, con su correspondiente entorno
en `platformio.ino`

# Pruebas

Bastante mal. El robot avanza correctamente unos 40 cm, luego gira más o menos
los 180 grados pero la siguiente recta no es ni mucho menos recta, si no bastante
torcida... Habrá que estudiarlo con calma. Nos parece que es debido al error
acumulado, que cuando se cambia de movimiento se opone a la corrección debida. Pero
antes de resetear este error, mañana intentaremos afinar los parámetros del PID.

commit: 9c6ba7edb0140db2b2187f926f58d1155463c25e
