---
layout: post
title:  "Empezando con flood fill"
commit: 290fb81c58b84f4c45301588e886ab11f83b617a
---

Aún nos quedaba por documentar `laberinto.cpp`, `robot.cpp` y `timer`.cpp`, pero no
pudimos resistirnos y empezamos a implementar el algoritmo de _flood fill_, aunque sea
de una manera simple.

Creamos su propio fichero `flood.cpp`, que contendrá el array de distancias, y será
independiente de `laberinto.cpp`. Tenemos una funcion para inicializarlo (suponiendo
un laberinto sólo con las paredes exteriores), y una función `flood_recalcula()`, que
recorrera **todas** las casillas del laberinto actualizando los pesos cuando alguno
esté mal. La función devuelve _true_ o _false_ en función de si hubo alguna actualización,
y la idea (_provisional_) es que en la rutina de control del robot, cuando esté parado,
se pueda llamar a esta función en cada interrupción. Y sólo arrancará cuando todos los
pesos esten correctos. Como norma, calculamos que se harán unas 50 pasadas, a 4ms cada
una, el tiempo no debería de notarse.

De todos modos lo correcto sería ir apilando las casillas en las que aparecen paredes nuevas y,
uando una casilla se actualiza, apilar a su vez sus vecinas. Pero queríamos hacer algo rápido.

También creamos algunas funciones auxiliares autoexplicativas:

```cpp
uint8_t flood_minimo_vecino(uint8_t casilla);
uint8_t flood_mejor_vecino_desde(uint8_t casilla);
void flood_encuentra_vecinos(uint8_t casilla);
```

# Problema extraño en los leds

Hemos aumentado el tiempo de espera en el muestreo de los leds hasta los 100 ms, a ver si
somos capazes de detectar **siempre** la pared frontal en el momento de entrar en la casilla,
y no tener que disparar el evento `accion_cancelar()`. Y
nos ha pasado una cosa rara: parecía como que hubiese interferencias entre todos los leds.
Poniéndo un obstáculo únicamente visible por uno de ellos, todos los demás aumentaban su
lectura (y bastante). Se ha solucionado con un reset, y no ha vuelto a pasar. Pero nos
quedamos con la mosca detrás de la oreja.


# Primeras pruebas de cálculo de distancias

Dejamos igualmente el algoritmo de la mano derecha, y sólo calculamos el flood cuando estamos
en el estado EXPLORANDO_INICIAL (es decir, parados). Cuando está todo listo, arrancamos, y
no volveremos a calcular hasta que haya una parada.

```cpp
case EXPLORANDO_INICIAL:
    Serial.println(F("explorando inicial"));
    if (!flood_recalcula()) {
        accion_ejecuta(ARRANCAR);
        sinc_pared = false;
        robot.casilla_offset = LABERINTO_LONGITUD_CASILLA / 2;
        robot.estado = EXPLORANDO;
    }
    break;
```

Al haber cambiado el tiempo de espera en los leds, el pid de corrección en función de los 
sensores de pared era un poco más brusco (los valores devueltos por los sensores han aumentado).
Lo hemos corregido un poco 'a mano', al igual que los umbrales de detección. Está todo bastante
cogido por los pelos y unas veces se hace bien y otras se hace mal. Habrá que empezar a logear
valores de sensores. 

También hemos tenido que cambiar las decisiones de giro en función de lo
almacenado en el laberinto, ya que la información de los sensores es demasiado compleja cuando
estamos enfrente de una pared como para interpretarla únicamente con un umbral. Se supone
que es mucho mejor la información que tenemos justo al entrar en una casilla (aunque, definitivamente,
hay que darle varias vueltas a esto).


# Ejemplo

```cpp
+-----+-----+-----+-----+
|.  3  .  2  .  1  .  0 | 
|                       |     
+     +-----+     +-----+
|.  4 |   3  .  2 |.  5 | 
|     |           |     |     
+     +-----+     +     +
|.  5  .  4  .  3  .  4 | 
|   >                   |     
+     +-----+     +-----+
|.  6 |.  7 |.  4 #.  0 | 
|     |     |     #     |     
+     +     +     +-----+
|.  7 |.  6  .  5  .  6 | 
|     |                 |     
+-----+-----+-----+-----+
```

Aquí el robot llevaba varias pasdas por el laberinto. Se ve que los pesos de flood fill están
bien. El laberinto está mal mapeado en la pared marcada con '#'. Esa pared no existe pero el robot
la ha marcado. Es debido a los umbrales de los sensores, pero eso es una batalla para otro día.
