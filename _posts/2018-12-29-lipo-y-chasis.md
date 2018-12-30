---
layout: post
title:  "LiPo y chasis"
---
## LiPo

Vamos a alimentar nuestro robot con una batería LiPo de dos celdas, unos
7.4v. Aunque los motores son de 6v, limitaremos el PWM máximo para no
llegar a los límites. Aunque no es exactamente lo mismo alimentar el
motor a 6v que a 7.4v con un ciclo del 80% de pwm, confiamos en que
no sufran demasiado.

Disponíamos de estas baterías, así que son las que vamos a utilizar.

[ZOP Power 7.4v 1000mAh](https://www.banggood.com/ZOP-Power-7_4V-1000mAh-2S-25C-Lipo-Battery-JST-Plug-p-955391.html?rmmds=search&cur_warehouse=CN)

Para un robot de altas prestaciones esta batería sería algo pesada, pero
para nuestro pequeño robot experimental parece aceptable. El conector
JST resulta ser algo incómodo de usar, algo a tener en cuentra para futuras
versiones.

## Chasis

Para empezar a ver a nuestro robot moviéndose, necesitamos poner lo que tenemos
hasta ahora en algún tipo de chasis. Vamos a utilizar una baquelita de 7x9cm,
a la que le soldaremos unos pines hembra para el arduino y el puente H, un
conector JST para la batería y un pequeño interruptor para poder desconectar
la batería fácilmente y diseñamos en [FreeCad](https://www.freecadweb.org/)
un pequeño chasis en el que atornillar la baquelita, unos pequeños soportes
para los motores y una bola loca de apoyo.

Los archivos de esta versión están en el la carpeta 3d del código fuente, aunque
tienen bastantes problemas:

- No hemos calculado bien la disposición de los taladros para la baquelita en
la pieza baquelita-mount. Hemos pensado que eran simétricos y no lo son: así que
hubo que tirar de soldador para desplazar los agujeros.
 
- Los soportes del motor han resultado ser un poco blandenges y giran sobre sí
mismos en el chasis. Por ahora pueden funcionar, pero es un mal diseño.

- Uno de los motores tiene cierta holgura en el soporte impreso, así que improvisamos
un nuevo agujero en el frontal en el que atornillar un pequeño tornillo m3 que apriete el
motor contra el plástico. También es un mal diseño, pero tenemos ganas de empezar
con la programación así que va a quedar así por ahora.

## Conexiones

Las conexiones son triviales: el puente H al arduino y los motores tal como se
explica en el post anterior, el negativo de la batería a tierra, y el positivo
al una pata del interruptor. La otra pata irá al Vin del arduino.

# Estado actual

Por ahora, nuestro robot tiene la siguiente apariencia: hay un buzzer, diodos y
switchs en la placa que por ahora están sin conectar e iremos viendo si los utilizamos
o no. Además hay conectores frontales para los futuros sensores y una conexión para
bluetooth que iremos viendo.

<img src="{{ site.baseurl }}/images/adefesio-bottom-1.jpg" alt="adefesio parte inferior"/>
<img src="{{ site.baseurl }}/images/adefesio-top-1.jpg" alt="adefesio parte superior"/>
