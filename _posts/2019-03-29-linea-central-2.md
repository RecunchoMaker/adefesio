---
layout: post
title:  "Recuperar la línea central - 2"
commit: b4d156d16a7640c2b8ceb97fcfad77a4150ec7a7
---

Nos hemos liado un poco con los signos en el los cálculos pero al final lo hemos 
conseguido.

De todos modos, hemos tenido que poner una constante (hardcodeada por ahora) en
la corrección del ángulo basado en el diferencial de lecturas. Provoca demasiado
balanceo en la corrección, lo que nos hace pensar que hay algún error en los cálculos,
pero que no encontramos. Quizá estos cálculos están demasiado sobrecargados y bastaba
con otro pid que se basara en la distancia, con componente proporcional y derivada...
Quizá otro día probemos esto.

Ahora la función `motores_actualiza_velocidad()` tiene en cuenta la corrección del
ángulo que se obtiene en `robot.cpp` calculando el radio de la curva que tiene
que hacer para corregirse:

```cpp
velocidad_angular_objetivo = angulo * motores_get_velocidad_lineal_objetivo() / DISTANCIA_CONVERGENCIA;
radio_aux = velocidad_lineal_objetivo / velocidad_angular_objetivo;

velocidad_lineal_objetivo_left = velocidad_angular_objetivo * (radio_aux + distancia_entre_ruedas/2);
velocidad_lineal_objetivo_right = velocidad_angular_objetivo * (radio_aux - distancia_entre_ruedas/2);
```

# Filtro Kalman para distancias

Utilizamos un pequeño filtro kalman para suavizar el ruido en los leds. Inicialmente lo introdujimos
para comprobar si el balanceo al introducir el ángulo del diferencial (segunda componente en el cálculo
que aparece más abajo). Quizá no sea necesario pero vamos a dejarlo.

# Fiarse de la pared siguiente en modo exploración

Antes teníamos un pequeño control para no utilizar la pared de la siguiente casilla,
ya hasta que llegamos a ella no sabemos si tiene pared o no. Sin embargo, ya que tenemos
calculada la distancia a las paredes, no utilizamos el valor almacenado en el array _laberinto_,
si no que utilizamos directamente la distancia de los leds si es inferior a un margen,
y controlando que el diferencial no sea demasiado elevado, lo que indicaría que estamos
detectando ya una vigueta y el valor no es correcto.

```cpp
if (leds_get_distancia_kalman(LED_IZQ) < 0.08 and leds_get_distancia_d(LED_IZQ) < 0.002) {
    desvio += (-leds_get_distancia_kalman(LED_IZQ) - (ANCHURA_ROBOT / 2.0) + (LABERINTO_LONGITUD_CASILLA/2.0)) / DISTANCIA_CONVERGENCIA;
    desvio -= 0.05 * (leds_get_distancia_d(LED_IZQ) / (PERIODO_CICLO * motores_get_velocidad_lineal_objetivo()));
}
if (leds_get_distancia_kalman(LED_DER) < 0.08 and leds_get_distancia_d(LED_DER) < 0.002) {
    desvio += (leds_get_distancia_kalman(LED_DER) + (ANCHURA_ROBOT / 2.0) - (LABERINTO_LONGITUD_CASILLA/2.0)) / DISTANCIA_CONVERGENCIA;
    desvio += 0.05 * (leds_get_distancia_d(LED_DER) / (PERIODO_CICLO * motores_get_velocidad_lineal_objetivo()));
    if (leds_pared_izquierda()) desvio /= 2.0;  // si contamos dos veces
}
```
