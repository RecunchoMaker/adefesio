---
layout: post
title:  "PID básico"
commit: 26f78cf3df3fae6e5678fbace545951847f6dfdf
---
Tenemos una interrupción de tiempo que nos actualiza una variable con los ticks
transcurridos entre dos saltos de encoder. Podemos utilzar estos ticks como
medida inversa de velocidad.

En la magnífica (charla)[https://www.youtube.com/watch?v=R1CZGU-SWsA&t=837s]
de los creadores de Bulebule, hablan sobre la conveniencia de utilizar unidades
de medida estándard (metros por segundo). Seguro que es cierto, pero como
el Atmega328p es bastante menos potente que el STM322F, queremos evitar en lo posible
operaciones de coma flotante. 

# PID básico

Dados unos ticks entre saltos de encoder "obtenidos" y "deseados" vamos a probar
un PID básico que calcule un nuevo pwm en función del error únicamente con la constante proporcional.
```
nuevo_pwm = (ticks_obtenidos - ticks_deseados) * Kp
```

Estuvimos haciendo algunas pruebas con este tipo de pid y no nos convence. Supongamos
que tenemos unos ticks actuales de 30000 y queremos aumentar la velocidad a 20000 ticks.
Hay un error de 10000 ticks que ciertamente no tiene que ver con el pwm que queremos poner
en el motor: son distintas unidades. El caso de pasar de 40000 a 30000 nos daría el mismo
valor de pwm y no tiene que ver un caso con el otro, al menos es lo que pensamos.

Siempre podríamos arreglarlo con el error acumulado y una constante integral, pero
nos gustaría acercarnos al objetivo, aunque sea de forma pendular,  únicamente con la constante Kp.

Así que lo que vamos a hacer es calcular "el incremento de pwm", dado el pwm actual
y una constante Kp.
```
    error_left = ticks_deseados_left - encoders_get_ticks_left();
    error_right = ticks_deseados_right - encoders_get_ticks_right();

    motores_set_pwm(pwm_left - error_left * KP, pwm_right - error_right * KP);
```

Presntamos unas muestras obtenidas con este algoritmo:

```
deseados: 25000   obtenidos: 31998   pwm: (164, 158)  pasos_left = 316   pasos_right = 336   desviacion entre ruedas = 20 
deseados: 26000   obtenidos: 31998   pwm: (168, 161)  pasos_left = 316   pasos_right = 335   desviacion entre ruedas = 19 
deseados: 26000   obtenidos: 15999   pwm: (164, 161)  pasos_left = 301   pasos_right = 333   desviacion entre ruedas = 32 
deseados: 27000   obtenidos: 31998   pwm: (165, 161)  pasos_left = 305   pasos_right = 333   desviacion entre ruedas = 28 
deseados: 27000   obtenidos: 31998   pwm: (155, 147)  pasos_left = 293   pasos_right = 302   desviacion entre ruedas = 9 
deseados: 28000   obtenidos: 31998   pwm: (159, 146)  pasos_left = 286   pasos_right = 301   desviacion entre ruedas = 15 
deseados: 28000   obtenidos: 31998   pwm: (145, 140)  pasos_left = 277   pasos_right = 292   desviacion entre ruedas = 15 
deseados: 29000   obtenidos: 31998   pwm: (148, 145)  pasos_left = 276   pasos_right = 293   desviacion entre ruedas = 17 
deseados: 29000   obtenidos: 31998   pwm: (143, 142)  pasos_left = 264   pasos_right = 292   desviacion entre ruedas = 28 
deseados: 30000   obtenidos: 31998   pwm: (142, 142)  pasos_left = 266   pasos_right = 293   desviacion entre ruedas = 27 
deseados: 30000   obtenidos: 31998   pwm: (138, 128)  pasos_left = 252   pasos_right = 256   desviacion entre ruedas = 4 
deseados: 31000   obtenidos: 31998   pwm: (138, 121)  pasos_left = 253   pasos_right = 255   desviacion entre ruedas = 2 
deseados: 31000   obtenidos: 31998   pwm: (139, 119)  pasos_left = 247   pasos_right = 253   desviacion entre ruedas = 6 
deseados: 32000   obtenidos: 31998   pwm: (138, 123)  pasos_left = 250   pasos_right = 251   desviacion entre ruedas = 1 
deseados: 32000   obtenidos: 31998   pwm: (135, 115)  pasos_left = 241   pasos_right = 238   desviacion entre ruedas = -3 
deseados: 33000   obtenidos: 31998   pwm: (132, 112)  pasos_left = 241   pasos_right = 237   desviacion entre ruedas = -4 
deseados: 33000   obtenidos: 31998   pwm: (130, 118)  pasos_left = 233   pasos_right = 236   desviacion entre ruedas = 3 
deseados: 34000   obtenidos: 31998   pwm: (132, 116)  pasos_left = 234   pasos_right = 237   desviacion entre ruedas = 3 
deseados: 34000   obtenidos: 31998   pwm: (130, 119)  pasos_left = 229   pasos_right = 234   desviacion entre ruedas = 5 
deseados: 35000   obtenidos: 47997   pwm: (127, 117)  pasos_left = 228   pasos_right = 234   desviacion entre ruedas = 6 
deseados: 35000   obtenidos: 31998   pwm: (125, 111)  pasos_left = 224   pasos_right = 222   desviacion entre ruedas = -2 
deseados: 36000   obtenidos: 31998   pwm: (125, 112)  pasos_left = 222   pasos_right = 223   desviacion entre ruedas = 1 
deseados: 36000   obtenidos: 31998   pwm: (123, 112)  pasos_left = 219   pasos_right = 223   desviacion entre ruedas = 4 
deseados: 37000   obtenidos: 31998   pwm: (126, 112)  pasos_left = 217   pasos_right = 223   desviacion entre ruedas = 6 
deseados: 37000   obtenidos: 31998   pwm: (119, 109)  pasos_left = 213   pasos_right = 217   desviacion entre ruedas = 4 
deseados: 38000   obtenidos: 47997   pwm: (118, 112)  pasos_left = 213   pasos_right = 217   desviacion entre ruedas = 4 
deseados: 38000   obtenidos: 31998   pwm: (119, 107)  pasos_left = 208   pasos_right = 210   desviacion entre ruedas = 2 
deseados: 39000   obtenidos: 47997   pwm: (114, 110)  pasos_left = 207   pasos_right = 209   desviacion entre ruedas = 2 
deseados: 39000   obtenidos: 31998   pwm: (111, 107)  pasos_left = 203   pasos_right = 208   desviacion entre ruedas = 5 
```

Curiosamente, cuando se solicita una velocidad más rápida el sistema se comporta peor (nos parece que una diferencia
de 20-30 pasos en el medio segundo que hay entre cada línea mostrada no es aceptable): pero a velocidades lentas parece
comportarse bien. Aún faltan corregir la velocidad angular teniendo en cuenta en cada salto del timer la diferencia
de velocidades, y quizá introduzcamos una constante de integración. Aún está por ver.

De todos modos, decidimos pararnos aquí porque notamos que necesitamos algún sistema para ver gráficamente todas las
pruebas que vamos haciendo. Para sacar la muestra anterior estuvimos probando distintas constantes Kp y siendo sinceros,
nos parece un poco caótica esta forma de probar las cosas en con el bucle { cambiar parametros / compilar / ver números }.
Creemos que va a ser una inversión en tiempo el tener la posibilidad de visualizar gráficamente los resultados.
