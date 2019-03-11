---
layout: post
title:  "Ángulo sólo con pasos y distancia entre ejes"
commit: 345d7bcf6c6df7b9f347683bec1c67c3cbf8d937
---

**Nota:** le hemos cambiado el nombre a la constante _DISTANCIA ENTRE EJES_. Deberia
llamarse _DISTANCIA ENTRE RUEDAS_.

Hemos decidido ser menos ambiciosos en el cálculo del ángulo y, en vez
de calcular la distancia en función la velocidad, utilizar directamente
los pasos del encoder, es decir, usar la misma igualdad pero de otra manera:

```cpp
if (encoders_get_posicion_left() != encoders_get_posicion_right) {
    angulo_actual += (LONGITUD_PASO_ENCODER * encoders_get_posicion_left() -
                      LONGITUD_PASO_ENCODER * encoders_get_posicion_right())
                      / 
                      DISTANCIA_ENTRE_RUEDAS;
}

```

La comparación es simplemente para evitar un cálculo cuando ambas ruedas avancen
lo mismo. Nos gustaba la opción de ayer porque nos parecía más correcta, a sabiendas
de que habría errores acumulados, pero de esta manera debería ser prácticamente
perfecto, aunque la resolución sea menor.

Para nuestra sorpresa, el robot se sigue pasando de largo prácicamente los mismos
grados que de la otra manera (quizá algo menos), así que revisando las constantes implicadas vemos
que una variación de la distancia entre ruedas de 1 mm provoca una variación del 1%
en el cálculo.

# Problema en el chasis y distancia entre ruedas

Como comentamos en el post del chasis, éste no es muy robusto: uno de los motores
tiene cierta holgura y además con el tiempo parece que las ruedas "se abren" un poco, como
los _fórmula 1_. Esto hace que la rueda apoye mayormente en la parte interior, con lo
cual la distancia del centro del eje al punto de apoyo disminuye.

Así que, de una manera un poco _ad hoc_, introducimos una [constante de Shinner](https://www.ugr.es/~jmaroza/anecdotario/chciencia.htm),
que calculamos de forma empírica. Únicamente con dos milimetros menos el robot
gira exactamente 90 grados. Haciéndolo 12 veces seguidas se nota una desviación prácticamente
imperceptible, pero no creemos que tenga sentido afinar más, sobre todo teniendo en
cuenta el margen de error que introduce este chasis defectuoso.

# Diferencia en ambos cálculos

Hicimos algunas pruebas para comprobar la diferencia entre este cálculo y el de ayer,
ya con la distancia ajustada, y la de ayer sigue fallando, aumentando más lentamente. Volveremos
sobre esto porque nos gustaría aplicar el cálculo del ángulo utilizando la velocidad.
Pero por el momento lo dejamos así (y empieza a ser una frase recurrente al final de los
posts).
