---
layout: post
title:  "Control de batería"
---

Las baterías LiPo no deben descargarse más de un cierto umbral, para asegurar
una larga vida útil. Si una celda baja por debajo de los 3.0v ya no volverá a
recargarse, así que deberemos cargarla cuando el voltaje empieze a descender
un par de décimas de voltio desde el nominal 3.7v (considerado carga media).

Aunque es conveniente testear de vez en cuando con un testeador el voltaje de
ambas celdas de nuestra LiPo, vamos a incorporar un pequeño control en el robot
para avisarnos de que el voltaje total está entrando en un rango peligroso.

El conversor ADC del arduino tiene un rango de 0v a 5v, así que vamos a soldar dos
reseistencias de 10K entre el positivo del interruptor y tierra, y leeremos
desde el pin A7 del arduino el punto medio de ambas resistencias. Lo que 
estamos haciendo es dividir el voltaje de la batería entre dos (perdemos algo
del rango dinámico del conversor, pero consideramos esa precisión suficiente).
Tenemos que tener en cuenta que no estamos midiendo "celda por celda". Cuando
el voltaje leído por el arduino esté por debajo de 3.5 (teniendo en cuenta
que tenemos dos celdas y pero estamos dividiendo el voltaje total de la 
batería entre dos) el robot entrará en bucle, mostrando por el puerto serie
el mensaje de batería agotada y encendiendo y apagando el led incorporado
en el arduino.

Creamos las siguientes funciones en bateria.cpp

```cpp
void bateria_init(void);
float bateria_voltaje(void);
bool bateria_agotada(void);
void bateria_muestra_nivel(void);
void bateria_watchdog(void);

```

Controlaremos de vez en cuando la batería con el _battery tester_ e iremos viendo
si estas funciónes se comporta bien para controlar la carga. Tanto la función bateria_init()
como bateria_muestra_nivel() llaman a la función de watchdog que se encargará de
bloquear la ejecución en caso de que la batería baje del umbral definido en el
fichero de cabecera.

## Primer bug

Nos hemos dado cuenta que no tiene mucho sentido enviar por el puerto serie el valor del voltaje
si el robot está funcionando a batería... porque lógicamente no está conectado
al puerto serie.

En teoría, con nuestro esquema de conexiones, podríamos encender el interruptor con el puerto
usb conectado al arduino. Sin embargo, preferimos no tentar a la suerte por si en algún 
momento se produce alguna mala conexión y la corriente va por donde no debiera, y acabamos
estropeando algo. Próximamente añadiremos un módulo bluetooth para poder depurar de
forma inalámbrica.

## Caso especial cuando se depura por USB

Además, el umbral que vamos a ponerle al control de batería (7.2v) se va a activar cuando
estemos con pruebas directamente conectados al USB del ordenador (que en nuestro caso nos
devuelve una lectura de unos 4.6v). Como este valor no se va a dar nunca en una LiPo de
dos celdas, consideramos en las funciones que este valor se corresponde con una conexión
directa al ordenador consecuentemente no bloqueamos la ejecución.

## Enlaces
[A guide to undesrtanding LiPo batteries) (https://rogershobbycenter.com/lipoguide/)
