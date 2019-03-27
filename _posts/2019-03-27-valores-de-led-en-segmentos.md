---
layout: post
title:  "Valores de led en segmentos"
commit: 472670bb53cda60a62025b361184bd4e7dfe2b58
---

Para traducir el valor devuelto por los sensores a un valor en milímetros tenemos
que guardar los datos de la curva que obtuvimos e interpolar. Pero 200 valores son
demasiados para lo que nos queda de memoria. Los guardamos en una matriz que tenga
un tamaño potencia de 2, para poder hacer un desplazamiento del valor del ADC y
acceder directamente a los valores de esa matriz, e interpolar.

Hacemos un pequeño script en python que lea el fichero de sensores y nos genere
el array para pegarlo directamente en el programa de arduino. Decidimos usar una
resolución de 4 bits, es decir, tendremos 16 valores (realmente 17 porque guardamos
tanto el inicial como el final para simplificar el cálculo) Nos sale esto:

```cpp
// Constantes generadas por leds_obtener_matriz_segmentos.py
const uint8_t leds_segmentos[17] = { \
     199 , 97 , 69 , 54 , 44 , 37 , 32 , 27 ,  \
     24 , 21 , 18 , 15 , 13 , 11 , 8 , 2 ,  \
     0 };

```

# Función leds_get_distancia()

Por fin podemos eliminar esa función que habíamos creado y que calculaba (es un decir)
la distancia a base de ifs. Ahora interpolamos la distancia en función de los dos valores
del array que contienen un valor menor y mayor que la lectura (y a los que podemos acceder
directamente desplazando 6 bits el valor de la lectura (10 - 4 = 6)

```cpp
/**
 * @brief Devuelve un valor estimado en mm a partir de la lectura analogica
 */
float leds_get_distancia(int8_t led) {
    
    int16_t lectura = leds_valor[led - A0];
    int8_t indice = leds_valor[led - A0] >> LEDS_BITS_INDICE_MUESTRA;
    float pendiente = (float) (leds_segmentos[indice+1] - leds_segmentos[indice]) / LEDS_ESPACIO_MUESTRA;
    int16_t espacio = (1 << LEDS_BITS_INDICE_MUESTRA) * indice;

    return (float) leds_segmentos[indice] + pendiente * (lectura - espacio);

}
```

Las pruebas parecen correctas. Hay una diferencia de algunos milímetros, pero nos parece más que suficiente.
Por lo menos por ahora, que nuestra idea es utilizar _la diferencia_ de una lectura a otra para calcular
el ángulo de nuestra trayectoria.
