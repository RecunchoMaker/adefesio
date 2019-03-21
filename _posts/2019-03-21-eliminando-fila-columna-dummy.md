---
layout: post
title:  "Eliminando fila y columna dummy"
commit: a08863dd77c071daf9be45d89a2cf9c464905bc8
---

Decíamos en un [post anterior](/adefesio/guardando-el-laberinto/) que era buena idea añadir
una fila/columna dummy en la que guardar las paredes más al oeste/sur, y simplificar 
comparaciones. Hemos cambiado de opinión.

El ahorro no era mucho, e introducía otras complicaciones: el eterno _+1_ cuando tratamos
con número de filas y columnas. Ej:

```cpp
for (idx = 0; idx < (laberinto_get_columnas() + 1)*(laberinto_get_filas() + 1) - 1; idx++) {
    flood_distancia[idx] = abs ( idx / (laberinto_get_columnas() + 1) -
                                 solucion / (laberinto_get_columnas() + 1))
                           +
                           abs ( idx % (laberinto_get_columnas() + 1) -
                                 solucion % (laberinto_get_columnas() + 1));
}
```

Ahora ya estaba hecho y podría quedar, pero este enfoque tiene un problema grave. En un
laberinto de 16x16, los números de las últimas casillas son mayores que 255, con lo cual
cualquier variable que contenga números de casilla (incluidas las futuras colas para un
tratamiento más rápido del flood) tendrían que ser de 16 bits.

Refactorizamos todo el código para guardar el laberinto en una matriz exacta de filas x
columnas. Y utilizaremos lógica adicional para, en el caso de las casillas más al sur
o al oeste, averiguar si hay pared (ya que no existe el componente SUR ni OESTE en nuestras
celdas). El código anterior queda así:

```cpp
for (idx = 0; idx < laberinto_get_columnas() * laberinto_get_filas(); idx++) {
    flood_distancia[idx] = abs ( idx / laberinto_get_columnas()) -
                                 solucion / laberinto_get_columnas()
                           +
                           abs ( idx % laberinto_get_columnas() -
                                 solucion % laberinto_get_columnas() );
}

```

Creemos que es mucho más claro y menos propenso a errores.
