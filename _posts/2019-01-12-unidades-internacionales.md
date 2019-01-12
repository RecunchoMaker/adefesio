---
layout: post
title:  "Unidades internacionales"
---

Vamos dar un paso atrás para definir las constantes implicadas en los cálculos utilizando
medidas internacionales (metros, segundos, radianes...) y reescribir las funciones en
consecuencia. Creemos que así tendremos unos datos más intuitivos y fáciles de
interpretar

# Medidas directas con calibre

Medimos el tamaño de las ruedas y la distancia entre ejes y las introducimos en el fichero
`settings.h', en metros.

```cpp
#define RADIO_RUEDAS 0.022
#define DISTANCIA_ENTRE_EJES 0.093
```

# Pasos por vuelta

Ya habíamos estimado los pasos por vuelta al empezar a usar los encoders, de forma empírica:
716 pasos por vuelta. Empezamos ahora a hacer más pruebas, contando varias las vueltas manualmente después
de poner un pwm en los motores y obteníamos un resultado no entero (entre 715 y 716) Por una parte,
estábamos (erróneamente) convencidos que la resolución del encoder iba a ser entera, pero nos dimos
cuenta que no tiene por qué ser así. Finalmente la solución estaba delante de nuestras narices en
las características del motor:  

[MicroMetal geared motor with encoder](http://www.volumerate.com/product/449751)

Se especifica claramente: *Hall Feedback Resolution: 357.7*. Es decir: 357.7 pasos por vuelta, lo que
coincide con las medidas que estábamos obteniendo

```cpp
#define PASOS_POR_VUELTA 357.7
#define LONGITUD_PASO_ENCODER ((2 * PI * RADIO_RUEDAS) / PASOS_POR_VUELTA)
```

# Medidas de tiempo

Cuando parametrizamos el timer, vamos a meter en el registro OCR1A el valor que debe alcanzar TCNT1
para disparar la interrupción. Este incremento de TCNT1 ocurre en un intervalo de tiempo que depende de la frecuencia del procesador (16Mhz)
y del preescalado (que por ahora siempre lo tendremos a 1). Nos interesa tener calculada ese intervalo
en segundos para el cálculo de velocidades.

```cpp
#define FRECUENCIA_TIMER 0.001
#define INTERVALO_TCNT1 (1/(OCR1A / FRECUENCIA_TIMER))
```

# Prueba: avanzar 1 metro

Hacemos un pequeño programa que avance un metro, con un control sencillo para que ambas ruedas
avancen la misma longitud. Vemos que rebasa un par de centímetros la distancia, así
que ponemos una pequeña deceleración final para asegurarnos que no es debido a la inercia.

```cpp
    while (encoders_get_posicion_right() * LONGITUD_PASO_ENCODER < 1.0) {
        if (encoders_get_posicion_right() * LONGITUD_PASO_ENCODER > 0.90) 
            pwm-=3;
        diff = encoders_get_posicion_left() - encoders_get_posicion_right();
        motores_set_pwm(pwm, pwm + diff);
        delay(100);
        Serial.print(encoders_get_posicion_right() * LONGITUD_PASO_ENCODER);
#ifdef ENCODERS_LOG_ESTADO
        encoders_log_estado();
#endif
    }
```

En sucesivas pruebas, el robot se para entre 0.99 y 1.01 metros, así qe damos el cálculo de la distancia por
bueno.

# Prueba: avanzar a 0.20 m/s

Ahora es mucho más fácil definir las funciones para especificar una velocidad, y, sobre todo, fácilmente
testeables con un cronómetro.

```cpp
void motores_set_velocidad(float velocidad_lineal, float velocidad_angular) {

    ticks_deseados_left = 
        (LONGITUD_PASO_ENCODER / velocidad_lineal) /
        INTERVALO_TCNT1;
    ticks_deseados_right = ticks_deseados_left;
}
```

Hemos cronometrado unos perfectos 0.20 m/s en un recorrido de un metro, teniendo en cuenta el error introducido
por el propio manejo manual del cronómetro. Damos por buenas las ecuaciones.

# Máximas y mínimas velocidades

A velocidades inferiores a 0.15 m/s el robot se comporta de forma errática, dándo saltos y desviándose muchísimo
de la trayctoria recta, y a velocidades superiores a 0.30 m/s, el pwm ya están los pwm prácticamente a tope. El 
límite superior es bastante lento (ya nos lo imaginamos cuando empezamos con los motores) pero no nos preocupa para
nuestro propósito principal que es aprender. El límite de 0.15 nos preocupa un poco más porque en los giros la rueda
interior irá bastante despacio y esto va a ser un problema. Probablemente tengamos que retomar el tema del cálculo de las velocidades
(sobre todo el caso 3)

# Y la reesritura de funciones?

Por ahora no vimos la necesidad de reescribir las funciones que teníamos: se encarga `motores_set_velocidad` de hacer
el cálculo inicial de ticks_deseados y todo funciona como antes. De todos modos, no descartamos la idea inicial de
reescribir las funciones.

commit: 7425f8947062ad675b9e9af7bd5a9e94e41af5dd
