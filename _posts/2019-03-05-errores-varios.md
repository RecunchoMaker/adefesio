---
layout: post
title:  "Errores varios"
commit: 0a50a40dd74b6df2cc3b6d411fc9ba286a69132f
---

Intentaremos dejar lo de ayer un poco más limpio y ordenado.

# Error en el cálculo de la distancia de frenado

La función del cálculo de distancia de frenado [**seguía**](adefesio/adefesio/bugs-en-robotscpp/) estando mal. Los paréntesis
estaban mal puestos y funcionaba de casualidad con algunas aceleraciones.

La (esperemos que) definitiva es:

```cpp
float _distancia_para_decelerar(float vi, float vf, float aceleracion) {
    return ((0.5 * (vi - vf) * (vi - vf)) + (vf * (vi - vf))) / aceleracion;
}
```

# Pared en la casilla de inicio

Las normas de la Oswdem establecen que el robot arranca de una casilla con tres paredes, pero para hacerlo más generalista
vamos a detectarla igualmente. La función que tenemos para setear las paredes (que recibe argumentos de pared frontal,
izquierda y derecha) la dividimos en dos: una para setear las paredes laterales, y otra para marcar la pared frontal.


# Medición de las distancias correctas

Hemos hecho pruebas, manteniendo una nueva variable en la ISR de los encoders, que no se reseteara nunca. Pudimos comprobar
que las distancias están correctas (la impresión de ayer era errónea). Hay una pequeña variación de 1 o 2 mm. en un
laberinto de 16 casillas, que parece ser debido a redondeos en la coma flotante. Eso lo dejamos por bueno.


# paredE -> paredO

Otro pequeño bug debido a que confundimos este con oeste... Realmente guardamos las paredes N y O de cada celda (redefinimos
los nombres, las asignaciones eran correctas)

# Otro error en los giros en redondo de 90 grados

Tal y como tenemos hecho el cálculo de distancias, suponemos que alcanzamos siempre la velocidad máxima que guardamos en la
acción. Esto hace que en un giro de 90 grados, si lo hacemos de forma análoga a los de 180:

```cpp
_crea_accion(PI*motores_get_distancia_entre_ruedas()/4, acur, acur, vc, vc , ROBOT_V0, GIRA90); 
```

... ocurre que no se alcanza la velocidad máxima, con lo cual se empieza a decelerar antes de tiempo y no completamos los 90 grados.
Por ahora ponemos la aceleración máxima y una velocidad constante de 0.2, pero habrá que volver sobre esto.

Dejamos el commit aquí. Quisimos hacer una implementación rápida del algoritmo de "mano derecha", y ha sido divertido como el
robot tiraba todas nuestras paredes. Aunque se adivinaba cierta intención. 
