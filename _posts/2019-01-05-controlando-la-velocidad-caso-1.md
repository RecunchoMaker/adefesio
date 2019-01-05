---
layout: post
title:  "Controlando la velocidad - caso 1"
---
Ayer hemos tenido que dejar el commit bastante sucio por falta de tiempo y hemos sacado una
primera muestar de datos que mostraba algunos resultados un tanto extraños. Vamos a depurar
lo que hicimos ayer, evaluando cada caso uno por uno.

Comenzamos con el caso 1, que se produce cuando se disparan varias veces la interrupción del encoder entre
dos interrupciones de timer. En un principio parece un caso trivial: el numero de ticks
entre cada salto se corresponderá con la diferencia entre los valores guardados en 
nuestro buffer tcnt1_left[]. Es fácil de ver que cuando se produce este caso, el
segundo valor del buffer nunca puede ser menor que el primero, y sin embargo, esto
se produce en varias ocasiones:

```
caso:1   sin act:0   encoder left: 2   ticks: 10583   0,1 = 11378 10397
caso:1   sin act:0   encoder left: 2   ticks: 13969   0,1 = 15377 9345
caso:1   sin act:0   encoder left: 2   ticks: 13102   0,1 = 13359 6734
```

Vamos a poner un control (temporal) en `encoders_get_ticks_entre_saltos_left` de manera
que si el segundo valor del buffer es menor que el primero, muestre por el puerto serie los valores, 
precedidos de la palabra ERROR.

```
Error: caso:1   sin act:1   encoder left: 2   ticks: 63264   0,1 = 2285 13 
Error: caso:1   sin act:0   encoder left: 2   ticks: 58494   0,1 = 7091 49 
Error: caso:1   sin act:0   encoder left: 2   ticks: 58704   0,1 = 6849 17 
Error: caso:1   sin act:1   encoder left: 2   ticks: 62934   0,1 = 2610 8 
Error: caso:1   sin act:0   encoder left: 2   ticks: 58519   0,1 = 7076 59 
Error: caso:1   sin act:0   encoder left: 2   ticks: 58524   0,1 = 7067 55 
Error: caso:1   sin act:1   encoder left: 3   ticks: 59655   0,1 = 14637 8756 
Error: caso:1   sin act:0   encoder left: 2   ticks: 58359   0,1 = 7195 18 
Error: caso:1   sin act:1   encoder left: 2   ticks: 62380   0,1 = 3165 9 
Error: caso:1   sin act:1   encoder left: 2   ticks: 62632   0,1 = 2906 2 
Error: caso:1   sin act:0   encoder left: 2   ticks: 58615   0,1 = 7047 126 
Error: caso:1   sin act:0   encoder left: 2   ticks: 58451   0,1 = 7139 54 
Error: caso:1   sin act:1   encoder left: 2   ticks: 65210   0,1 = 5257 4931 
Error: caso:1   sin act:1   encoder left: 2   ticks: 62711   0,1 = 2852 27 
Error: caso:1   sin act:0   encoder left: 2   ticks: 58364   0,1 = 7192 20 
Error: caso:1   sin act:0   encoder left: 2   ticks: 58481   0,1 = 7080 25 
Error: caso:1   sin act:0   encoder left: 2   ticks: 58246   0,1 = 7292 2 
Error: caso:1   sin act:0   encoder left: 2   ticks: 58262   0,1 = 7301 27 
```

Aprece un patron interesante: en la mayoría de los casos, el segundo valor es bastante pequeño. Suponíamos
que la interrupción de timer se ejecutaría cuando TCNT1 llega a 0, pero parece que no es así, ya que la 
posicion del encoder no se ha reseteado. Vamos a almacenar
en unas variables `min_tcnt1` y `max_tcnt1` el mímimo TCNT1 al inicio y el máximo TCNT1 al final de la interrupción
de tiempo. No pretendemos obtener exactitud, simplemente una idea de entre qué rangos se está ejecutando el timer.

```
volatile uint16_t min_tcnt1 = 15999;
volatile uint16_t max_tcnt1 = 0;

ISR (TIMER1_COMPA_vect) {

    if (TCNT1 < min_tcnt1) {
        min_tcnt1 = TCNT1;
        Serial.print("min :");
        Serial.println(min_tcnt1);
    }

    ticks = encoders_get_ticks_entre_saltos_left();
    encoders_reset_posicion();

    if (TCNT1 > max_tcnt1) {
        max_tcnt1 = TCNT1;
        Serial.print("max :");
        Serial.println(max_tcnt1);
    }
}
```

La prueba es un completo desastre y constantemente muestra un valor distinto de `min_tcnt1` mientras el máximo
va aumentando hasta acercarse a 15999... lo cual implicaría que la interrupción no se puede ejecutar en un único
milisegundo. Si hacemos dos pruebas distintas, una de ellas con el mínimo y otra con el máximo, aparecen unos
valores algo más congruentes (minimo alrededor de 55 y máximo sobre 200) Tomamos estos valores con bastante
reserva (seguramente hay algún error de base en estas pruebas), pero lo que parece claro es que el timer no
se ejecuta en TCNT1 = 0, lo cual hace que el valor del encoder no esté reseteado cuando TCNT1 da la vuelta,
resuntado que tcnt1_left[1] sea mayor que tcnt1_left[0].

Intentamos parchear la función de cálculo de ticks para el caso 1 añadiendo un control 
que no es muy correcto.

```
    if (encoder_posicion_left >=2) {
        // caso 1
        caso = 1;
        encoder_ticks = tcnt1_left[1] - tcnt1_left[0];
        if (encoder_ticks < 0)
            encoder_ticks += OCR1A;

        ticks_sin_actualizar_left = 0;
        last_encoder_ticks = encoder_ticks;

        return encoder_ticks ;
```

No nos gusta mucho, ya que no debería hacer falta, pero lo dejamos en la lista de cosas a revisar. Por
lo menos ahora, este caso parece que devuelve unos valores bastante proporcionales al PWM.

```
~$ [master>] pio device monitor | tee /tmp/prueba1.txt
~$ egrep "caso:1|pwm" /tmp/prueba1.txt
pwm = 160
caso:1   sin act:1   encoder left: 2   ticks: 10992   0,1 = 11240 10917 
pwm = 161
pwm = 162
caso:1   sin act:1   encoder left: 2   ticks: 10440   0,1 = 1255 11695 
pwm = 163
caso:1   sin act:1   encoder left: 2   ticks: 15657   0,1 = 244 15901 
caso:1   sin act:1   encoder left: 2   ticks: 10719   0,1 = 4930 15649 
caso:1   sin act:1   encoder left: 2   ticks: 10712   0,1 = 4789 15501 
pwm = 164
caso:1   sin act:1   encoder left: 2   ticks: 15179   0,1 = 15947 10668 
pwm = 165
pwm = 166
caso:1   sin act:1   encoder left: 2   ticks: 10352   0,1 = 725 11077 
pwm = 167
caso:1   sin act:1   encoder left: 2   ticks: 10265   0,1 = 4977 15242 
pwm = 168
caso:1   sin act:1   encoder left: 2   ticks: 10219   0,1 = 5482 15701 
pwm = 169
caso:1   sin act:1   encoder left: 2   ticks: 14931   0,1 = 15635 9823 
caso:1   sin act:1   encoder left: 2   ticks: 9882   0,1 = 14387 13016 
caso:1   sin act:1   encoder left: 2   ticks: 10335   0,1 = 1854 12189 
pwm = 170
caso:1   sin act:1   encoder left: 2   ticks: 14894   0,1 = 410 15304 
pwm = 171
caso:1   sin act:1   encoder left: 2   ticks: 9873   0,1 = 2827 12700 
pwm = 172
caso:1   sin act:1   encoder left: 2   ticks: 9708   0,1 = 9950 8325 
pwm = 173
pwm = 174
caso:1   sin act:1   encoder left: 2   ticks: 9548   0,1 = 5716 15264 
caso:1   sin act:1   encoder left: 2   ticks: 9959   0,1 = 2154 12113 
pwm = 175
caso:1   sin act:1   encoder left: 2   ticks: 9543   0,1 = 6259 15802 
caso:1   sin act:1   encoder left: 2   ticks: 9466   0,1 = 4027 13493 
pwm = 176
caso:1   sin act:1   encoder left: 2   ticks: 9414   0,1 = 14562 12472 
pwm = 177
caso:1   sin act:1   encoder left: 2   ticks: 9836   0,1 = 3758 13594 
pwm = 178
caso:1   sin act:1   encoder left: 2   ticks: 9525   0,1 = 5963 15488 
caso:1   sin act:1   encoder left: 2   ticks: 14059   0,1 = 14304 7622 
caso:1   sin act:1   encoder left: 2   ticks: 9759   0,1 = 4575 14334 
pwm = 179
pwm = 180
caso:1   sin act:1   encoder left: 2   ticks: 9493   0,1 = 4243 13736 
caso:1   sin act:1   encoder left: 2   ticks: 13524   0,1 = 2294 15818 
pwm = 181
caso:1   sin act:1   encoder left: 2   ticks: 9520   0,1 = 14293 11843 
caso:1   sin act:1   encoder left: 2   ticks: 9267   0,1 = 4993 14260 
pwm = 182
caso:1   sin act:1   encoder left: 2   ticks: 9253   0,1 = 9496 7425 
pwm = 183
caso:1   sin act:1   encoder left: 2   ticks: 9388   0,1 = 5864 15252 
caso:1   sin act:1   encoder left: 2   ticks: 9164   0,1 = 9406 6743 
caso:1   sin act:1   encoder left: 2   ticks: 9077   0,1 = 2722 11799 
pwm = 184
caso:1   sin act:1   encoder left: 2   ticks: 9299   0,1 = 12926 10234 
caso:1   sin act:1   encoder left: 2   ticks: 8869   0,1 = 9254 6394 
caso:1   sin act:1   encoder left: 2   ticks: 9102   0,1 = 4416 13518 
caso:1   sin act:1   encoder left: 2   ticks: 8908   0,1 = 9667 6795 
pwm = 185
caso:1   sin act:1   encoder left: 2   ticks: 9073   0,1 = 9318 6918 
caso:1   sin act:1   encoder left: 2   ticks: 13028   0,1 = 14986 8221 
pwm = 186
caso:1   sin act:1   encoder left: 2   ticks: 8966   0,1 = 5993 14959 
pwm = 187
caso:1   sin act:1   encoder left: 2   ticks: 8903   0,1 = 10105 7504 
pwm = 188
caso:1   sin act:1   encoder left: 2   ticks: 13024   0,1 = 14635 7555 
caso:1   sin act:0   encoder left: 2   ticks: 8858   0,1 = 14952 12068 
caso:1   sin act:0   encoder left: 2   ticks: 8846   0,1 = 6839 15685 
caso:1   sin act:1   encoder left: 2   ticks: 13232   0,1 = 1765 14997 
caso:1   sin act:1   encoder left: 2   ticks: 13229   0,1 = 2682 15911 
pwm = 189
caso:1   sin act:1   encoder left: 2   ticks: 8644   0,1 = 2697 11341 
caso:1   sin act:1   encoder left: 2   ticks: 12682   0,1 = 2736 15418 
pwm = 190
caso:1   sin act:1   encoder left: 2   ticks: 8642   0,1 = 10716 7483 
caso:1   sin act:1   encoder left: 2   ticks: 12601   0,1 = 13031 5954 
caso:1   sin act:0   encoder left: 2   ticks: 8852   0,1 = 6312 15164 
caso:1   sin act:1   encoder left: 2   ticks: 8941   0,1 = 12694 9502 
pwm = 191
caso:1   sin act:1   encoder left: 2   ticks: 8885   0,1 = 4835 13720 
caso:1   sin act:0   encoder left: 2   ticks: 8639   0,1 = 6227 14866 
pwm = 192
caso:1   sin act:1   encoder left: 2   ticks: 8437   0,1 = 5374 13811 
caso:1   sin act:0   encoder left: 2   ticks: 58119   0,1 = 7450 33 
pwm = 193
caso:1   sin act:1   encoder left: 2   ticks: 8430   0,1 = 9783 6323 
caso:1   sin act:0   encoder left: 2   ticks: 8374   0,1 = 7380 15754 
caso:1   sin act:1   encoder left: 2   ticks: 8729   0,1 = 9244 5706 
pwm = 194
caso:1   sin act:0   encoder left: 2   ticks: 8759   0,1 = 5739 14498 
caso:1   sin act:1   encoder left: 2   ticks: 8352   0,1 = 10292 6605 
caso:1   sin act:0   encoder left: 2   ticks: 8343   0,1 = 6592 14935 
pwm = 195
caso:1   sin act:1   encoder left: 2   ticks: 8422   0,1 = 2354 10776 
caso:1   sin act:0   encoder left: 2   ticks: 8394   0,1 = 6702 15096 
pwm = 196
caso:1   sin act:0   encoder left: 2   ticks: 8569   0,1 = 4800 13369 
caso:1   sin act:1   encoder left: 2   ticks: 12166   0,1 = 14705 7330 
caso:1   sin act:1   encoder left: 2   ticks: 8595   0,1 = 2644 11239 
pwm = 197
caso:1   sin act:1   encoder left: 2   ticks: 12469   0,1 = 1312 13781 
caso:1   sin act:0   encoder left: 2   ticks: 8393   0,1 = 13494 9949 
pwm = 198
caso:1   sin act:0   encoder left: 2   ticks: 8595   0,1 = 5870 14465 
caso:1   sin act:1   encoder left: 2   ticks: 8141   0,1 = 242 8383 
caso:1   sin act:1   encoder left: 2   ticks: 8266   0,1 = 269 8535 
caso:1   sin act:1   encoder left: 2   ticks: 8170   0,1 = 12616 8720 
pwm = 199
caso:1   sin act:1   encoder left: 2   ticks: 8250   0,1 = 2206 10456 
caso:1   sin act:1   encoder left: 2   ticks: 8519   0,1 = 10624 6564 
caso:1   sin act:1   encoder left: 2   ticks: 12304   0,1 = 13580 5658 
pwm = 200
caso:1   sin act:0   encoder left: 2   ticks: 8367   0,1 = 7237 15604 
caso:1   sin act:1   encoder left: 2   ticks: 12240   0,1 = 15063 7074 
caso:1   sin act:0   encoder left: 2   ticks: 8170   0,1 = 5303 13473 
```

commit: 4228944ba5f4baa09d568fbfbef823b9c4360296
