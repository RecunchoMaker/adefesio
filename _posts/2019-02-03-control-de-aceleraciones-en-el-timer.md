---
layout: post
title:  "Control de aceleraciones en el timer"
---

Tenemos más o menos claros los valores que tenemos que calcular para calcular
las aceleraciones y deceleraciones, y queremos introducir el control en la
rutina de interrupción del timer.

Nos gustaría poder _apilar_ una secuencia de movimientos, de la forma `AADAAI` donde cada carácter será una
acción (avanza, derecha, izquierda. En la interrupción se evalúa el string y
se decide la siguiente aceleración lineal y angular, hasta un número de pasos
de encoder. Cuando se llega a esos pasos, se reevalúa el string y se deciden
las siguientes aceleraciones y así sucesivamente.

En la fase de resolución este esquema sería suficiente, ya que los sensores sólo
se utilizan para corregir ligeramente una trayectoria previamente marcada. La fase de exploración
es distinta, porque la trayectoria se irá decidiendo en función de las lecturas... pero
eso será más adelante.

En el post [Posición del robot](../posicion-del-robot/) habíamos empezado a programar
unas funciones en `robot.cpp` que no nos están gustando nada. Descartamos todo y lo enfocamos de esta
manera:

```cpp
void robot_control() {
    if (encoders_get_posicion_total() >= pasos_objetivo)
        robot_siguiente_accion();
    else if (encoders_get_posicion_total() > pasos_hasta_decelerar)
        motores_set_aceleracion_lineal(robot_deceleracion);
    else
        if (motores_get_velocidad_lineal_objetivo_temp() > velocidad_maxima) {
            motores_set_aceleracion_lineal(0);
    motores_actualiza_velocidad()
}
```

Esta función se ejecuta a cada paso de interrupción, y `robot_siguiente_accion()` 
se ejecuta cuando una acción termina y hay que decidir una nueva aceleración lineal
y angular. Ahí se calcularán las variables `pasos_objetivo`, `pasos_hasta_decelerar`,
, `velocidad_maxima` y `deceleracion` de esa acción.

# _mock_ de secuencias

Creamos la estructura:

```cpp
struct tipo_accion {
    uint32_t pasos_objetivo;
    uint32_t pasos_hasta_decelerar;
    float deceleracion;
    float velocidad_maxima;
    float radio;
};
```

... y un array de acciones en las que intentaremos reproducir los movimientos que hemos
hecho hasta ahora en los últimos posts, pero haciendo que se encargue la rutina de control.
Con la línea de comandos, simplemente activamos la primera secuencia.

Por ahora sólo ejecutamos un movimiento rectilíneo de 50 cm.

```cpp
    distancia = 0.5;
    pasos = distancia / LONGITUD_PASO_ENCODER;
    deceleracion = 0.5;
    velocidad_maxima = 0.2;
    velocidad_final = 0;
    radio = 9999;

    accion_array[1].pasos_objetivo = pasos;
    accion_array[1].pasos_hasta_decelerar = (distancia - _distancia_para_decelerar(velocidad_maxima - velocidad_final, deceleracion)) / LONGITUD_PASO_ENCODER;
    accion_array[1].deceleracion = deceleracion;
    accion_array[1].velocidad_maxima = velocidad_maxima;
    accion_array[1].radio = radio;
```

Sólo lo probamos conectados al USB pero parece que el esquema va bien. En
posteriores días programaremos los demás movimientos y, por último, intentaremos
hacer una rutina que traslade el string de tipo `AADAAI` en acciones.

commit: 1ab11c71b75e2dd147842c1a08b089065b468e7d
