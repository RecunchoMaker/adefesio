---
layout: post
title:  "Controlando la velocidad"
commit: a875a17a3bd8c8ed5471409aed4aa4b3fae982cd
---
Con el timer preparado y los motores funcionando con sus encoders ya podemos 
empezar a controlar la velocidad de cada motor con un [PID](https://es.wikipedia.org/wiki/Controlador_PID).
Por ahora, no vamos a preocuparnos de la aceleración: sólo vamos intentar acercar los motores a la velocidad
requerida, e intentar encontrar unos parámetros relativamente aceptables para el PID.

# Obtener pasos de encoder por segundo

Programamos una primera prueba para mostrar los pasos de encoder que se disparan cada
segundo, a un pwm medio de 100, configurando nuestro timer con preescalado 256 e
imprimiendo en la ISR el valor de los encoders.

```cpp
ISR (TIMER1_COMPA_vect) {
    Serial.println(encoders_get_posicion_left());
    encoders_reset_posicion();
}

void setup() {
    Serial.begin(115200);
    bateria_init();
    motores_init();
    encoders_init();
    bateria_muestra_nivel();
    timer1_init(1, 256);
    sei();
    motores_set_pwm(00,0);
}

void loop() {
}
```

Se disparan unos 700-720 pasos por segundo. Vemos que si queremos actualizar la velocidad cada
ms no nos basta con comprobar el número de pasos, ya que a una velocidad media el número de pasos
por ms va a ser 0 ó 1, lo que nos dará saltos enormes de una iteracción a otra. Podríamos ir
acumulando los pasos y obtener una media en un intervalo de tiempo mayor, pero creemos que
perdemos mucha velocidad de reacción.

# Registro TCNT1

Cuando activamos el timer, el registro TCNT1 se va incrementando hasta alcanzar el valor de
comparación (ver enlaces en el post anterior). Vamos a quedarnos los dos últimos valores de
este registro en la ISR de los encoders para poder hacer el cálculo de la velocidad en la
ISR del timer. Necesitamos entonces un array de dos valores y, en cada llamada a la interrupción
del encoder, desplazamos el ultimo valor y anotamos el valor del contador TCNT1

```cpp
    // en encoders_IRS_left
    tcnt1_left[0] = tcnt1_left[1];
    tcnt1_left[1] = TCNT1;
```

Supongamos una linea temporal en la que los pulsos se corresponden con los disparos de la
interrupción de tiempo. Las fias etiquetadas con "Caso x" se corresponden con los disparos
del encoder

```
      +-----+         +-----+         +-----+         +-----+         +
      |     |         |     |         |     |         |     |         |  
      | 1ms |         | 2ms |         | 3ms |         | 4ms |         |  
    --+     +---------+     +---------+     +---------+     +---------+
Caso 1:       1     2 
Caso 2:       1                  2
Caso 3:       1                                    2

```

### Caso 1
El Encoder se dispara varias veces entre dos ticks de la interrupcion de reloj. Cuando se
ejecuta la interrupción de tiempo en el milisegundo 2, Suponiendo
como ejemplo el encoder izquierdo, el número de ciclos entre los dos ultimos saltos se corresponde con la diferencia 
entre los valores tnct1_left[1] - tnct1_left[0]

### Caso 2
Entre dos saltos de encoder se ejecuta una interrupción de reloj. Esto hace que la diferencia
entre los valores del array tnct1 no se corresponda con sea correcta (incluso puede ser negativa). Hay que sumarle el
número de ticks que se corresponde con un ciclo entero.

### Caso 3
De igual modo que en el caso 2, el número de ticks transcurridos entre ambos disparos de
encoder es la diferencia entre los elementos del array mas (número de tics por ciclo) * 2.
Esto sólo se hará en el milisegundo 4: en el milisegundo 2 y 3 tendremos que anotar que
no ha habido saltos de encoder e incrementaremos un contador.

# Cómo calcular el número de ticks por ciclo

Si reseteamos las variables encoder_posicion al final de la interrupción de reloj, nos
servirán para detectar el caso en el que estamos.

Programos la función `encoders_get_ticks_entre_saltos_left()`, que nos devolvera el numero
de ticks entre dos saltos, únicamente para el encoder izquierdo. Queremos asegurarmos de que
la idea es correcta antes de teclear el código para el otro encoder. Además, creamos unas
variables temporales en las que guardaremos los valores implicados en el cálculo, de manera
que podamos llamar en el loop principal a una rutina que imprima los valores cada cierto tiempo. Así
podemos coger una muestra de cómo se comportan nuestras fórmulas.

Aquí dejamos una muestra de las variables, indicando los tiempos calculados y el cálculo
de ticks.

```
pwm = 100
caso:2   sin act:0   encoder left: 1   ticks: 28552   0,1 = 10333 6887 
caso:2   sin act:0   encoder left: 1   ticks: 20176   0,1 = 13123 1301 
caso:2   sin act:0   encoder left: 1   ticks: 29242   0,1 = 12475 9719 
caso:3   sin act:1   encoder left: 0   ticks: 15999   0,1 = 10465 14486 
caso:2   sin act:0   encoder left: 1   ticks: 29197   0,1 = 10891 14171 
pwm = 110
caso:2   sin act:0   encoder left: 1   ticks: 28788   0,1 = 1721 14510 
caso:2   sin act:0   encoder left: 1   ticks: 26920   0,1 = 9119 4041 
caso:2   sin act:0   encoder left: 1   ticks: 25260   0,1 = 7577 9585 
caso:3   sin act:1   encoder left: 0   ticks: 15999   0,1 = 13413 14585 
caso:2   sin act:0   encoder left: 1   ticks: 17447   0,1 = 8245 9693 
pwm = 120
caso:2   sin act:0   encoder left: 1   ticks: 25533   0,1 = 1061 10595 
caso:1   sin act:0   encoder left: 2   ticks: 15436   0,1 = 227 15663 
caso:2   sin act:0   encoder left: 1   ticks: 15828   0,1 = 5128 12496 
caso:2   sin act:0   encoder left: 1   ticks: 16095   0,1 = 10278 10374 
caso:2   sin act:0   encoder left: 1   ticks: 22928   0,1 = 225 273 
pwm = 130
caso:2   sin act:0   encoder left: 1   ticks: 15916   0,1 = 341 258 
caso:2   sin act:0   encoder left: 1   ticks: 21390   0,1 = 3926 2364 
caso:2   sin act:0   encoder left: 1   ticks: 21282   0,1 = 2315 732 
caso:2   sin act:0   encoder left: 1   ticks: 21283   0,1 = 15276 13659 
caso:2   sin act:0   encoder left: 1   ticks: 14858   0,1 = 8555 7414 
pwm = 140
caso:2   sin act:0   encoder left: 1   ticks: 21070   0,1 = 2376 842 
caso:2   sin act:0   encoder left: 1   ticks: 19283   0,1 = 10344 7593 
caso:2   sin act:0   encoder left: 1   ticks: 13199   0,1 = 10980 8180 
caso:2   sin act:0   encoder left: 1   ticks: 13239   0,1 = 3840 7559 
caso:2   sin act:0   encoder left: 1   ticks: 19736   0,1 = 10938 14675 
pwm = 150
caso:2   sin act:0   encoder left: 1   ticks: 13329   0,1 = 13963 11293 
caso:2   sin act:0   encoder left: 1   ticks: 12473   0,1 = 5047 6593 
caso:1   sin act:0   encoder left: 2   ticks: 12072   0,1 = 2543 14615 
caso:2   sin act:0   encoder left: 1   ticks: 12231   0,1 = 11557 7789 
caso:1   sin act:0   encoder left: 2   ticks: 12101   0,1 = 3541 15642 
pwm = 160
caso:2   sin act:0   encoder left: 1   ticks: 17944   0,1 = 11225 7139 
caso:2   sin act:0   encoder left: 1   ticks: 11724   0,1 = 6919 2644 
caso:2   sin act:0   encoder left: 1   ticks: 16793   0,1 = 5179 524 
caso:1   sin act:0   encoder left: 2   ticks: 11543   0,1 = 932 12475 
caso:3   sin act:1   encoder left: 0   ticks: 15999   0,1 = 535 11969 
pwm = 170
caso:2   sin act:0   encoder left: 1   ticks: 16949   0,1 = 15756 10904 
caso:2   sin act:0   encoder left: 1   ticks: 10695   0,1 = 6089 5313 
caso:1   sin act:0   encoder left: 2   ticks: 10704   0,1 = 3411 14115 
caso:2   sin act:0   encoder left: 1   ticks: 10783   0,1 = 1066 258 
caso:1   sin act:0   encoder left: 2   ticks: 10583   0,1 = 11378 10397 
pwm = 180
caso:2   sin act:0   encoder left: 1   ticks: 15024   0,1 = 14112 8666 
caso:2   sin act:0   encoder left: 1   ticks: 14120   0,1 = 13189 11310 
caso:1   sin act:0   encoder left: 2   ticks: 9880   0,1 = 15547 13611 
caso:1   sin act:0   encoder left: 2   ticks: 13969   0,1 = 15377 9345 
caso:2   sin act:0   encoder left: 1   ticks: 14351   0,1 = 8474 2312 
pwm = 190
caso:1   sin act:0   encoder left: 2   ticks: 9706   0,1 = 5155 14861 
caso:2   sin act:0   encoder left: 1   ticks: 8950   0,1 = 5098 2722 
caso:1   sin act:0   encoder left: 2   ticks: 13102   0,1 = 13359 6734 
caso:1   sin act:0   encoder left: 2   ticks: 8969   0,1 = 1316 10285 
caso:2   sin act:0   encoder left: 1   ticks: 13190   0,1 = 15048 12239 
pwm = 200
caso:2   sin act:0   encoder left: 1   ticks: 13493   0,1 = 13075 10569 
caso:2   sin act:0   encoder left: 1   ticks: 8728   0,1 = 6947 3379 
caso:2   sin act:0   encoder left: 1   ticks: 12780   0,1 = 12285 4703 
caso:2   sin act:0   encoder left: 1   ticks: 8518   0,1 = 3832 468 
caso:2   sin act:0   encoder left: 1   ticks: 12407   0,1 = 11311 4137 
```

En principio, los resultados se aproximan a la realidad pero creemos que hay demasiada
diferencia en el resultado... Lamentablemente, se nos acaba el tiempo por hoy y tendremos
que dejarlo para otro día.
