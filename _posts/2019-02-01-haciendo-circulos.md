---
layout: post
title:  "Haciendo círculos"
---

Falta la parte que creemos más difícil. Hacer un giro de 90 grados, hacia adelante (no sobre sí mismo).
Empezamos intentando hacer un círculo de radio 9 cm, la mitad del tamaño de celda de un laberinto reglamentario.

El círculo exterior será 2*PI*(0.9 + DISTANCIA_ENTRE_RUEDAS/2), aproximadamente un metro. Teóricamente
(a nuestros 0.5 m/s en recto) podríamos hacer un círculo cada 2 segundos. Vamos a probar:

```cpp
motores_set_aceleracion_lineal(motores_get_maxima_aceleracion_lineal());
motores_set_aceleracion_angular(motores_get_maxima_aceleracion_angular());

while(1) {
    if (motores_get_velocidad_lineal_objetivo_temp() >= 2*PI*0.9 / 2) {
        motores_set_aceleracion_lineal(0);
    }
    if (motores_get_velocidad_angular_objetivo_temp() > PI) {
        motores_set_aceleracion_angular(0);
    }
    if (encoders_get_posicion_total_left() * LONGITUD_PASO_ENCODER > 2*PI*(0.9+(DISTANCIA_ENTRE_RUEDAS/2)))
        break;
}
```

Cuando arranca, volvemos a ver el "caballito". Nos damos cuenta que, al establecer una aceleración lineal, y
una acleración angular, ambas se suman en la rueda exterior, dando como resultado una aceleración
exagerada (dadas nuestras características de peso, centro de gravedad... etc). 

Rectificamos la aceleracion lineal con:

```cpp
    motores_set_aceleracion_lineal(
            motores_get_maxima_aceleracion_lineal()
            - (motores_get_maxima_aceleracion_angular() * PI * DISTANCIA_ENTRE_RUEDAS / 2));
```

Mucho mejor. El robot da unos círculos bastante buenos, quizá un pelín menos de 0.18 de diámetro, pero
constantes y muy bien de repetibilidad, aunque se pasa un poco de largo. Vamos a introducir la deceleración.

# Fallo en las ecuaciones para los giros

Intentando introducir la deceleración, el robot nos hace unas cosas raras al final. Nos damos cuenta del error:
estamos considerando la velocidad angular como independiente de la lineal, como si fuera "en parado". Cuando
el robot decelera (la velocidad lineal), manteniéndose la velocidad angular, en algún momento pasan a ser de
signos contrarios. Simplemente no tienen sentido esas ecuaciones en `motores_actualiza_velocidad()`

La velocidad angular no se mantiene constante cuando acelera, si no que tiene que aumentar y disminuir
directamente con la velocidad lineal (manteniendo el radio constante). El `set_aceleracion_angular` sólo
tiene sentido en el giro en redondo. 

Como la velocidad lineal es a la angular por el radio `v = w*r`, la velocidad de la rueda exterior es realmente
`w * (r+d)` y la interior `w * (r-d)` siendo _d_ la distancia entre ruedas / 2. Las ecuaciones del error para
el pid creemos que están bien, pero en vez de incrementar o decrementar la aceleración angular en función
de un valor que introduzcamos nosotros, lo que haremos es setear un radio y dejar en `motores_actualiza_velocidad()`
la siguiente asignación

```cpp
        // velocidad_angular_objetivo_temp += (aceleracion_angular * PERIODO_TIMER);
        velocidad_angular_objetivo_temp = velocidad_lineal_objetivo_temp / radio;
```

Ahora parece funcionar bien, aunque no hace un círculo completo... No sabemos de qué es. Mañana lo miramos
con calma.

commit: b128bb591b370c24db0c54e5b625ce9e5413ef41
