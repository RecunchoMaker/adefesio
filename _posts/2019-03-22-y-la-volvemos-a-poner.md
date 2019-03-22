---
layout: post
title:  "Y la volvemos a poner"
commit: 46a01d2748ca3dd117a74f68c5178e8c70158e21
---

Para comprobar si las casillas tienen pared al ESTE o al SUR, estamos haciendo una comprobación
para saber si la casilla en cuestión es la última, y si no, utilizamos el valor OESTE (o NORTE)
de la casilla adyacente.

```cpp
bool laberinto_hay_pared_este(uint8_t casilla) {
    return casilla % num_columnas == num_columnas - 1 or celda[casilla + CASILLA_ESTE].paredO;
}
```

Antes no lo hacíamos porque teníamos la fila/columna dummy que decidimos eliminar. Pero nos hemos
dado cuenta que, si el laberinto es de menos de 16x16, la comprobación no es necesaria. Por ejemplo,
en este laberinto de 4x5, en el que aparece el índice de las casillas correspondientes:

```
+----+----+----+----+----+
|0   |1   |2   |3   |4   |
|    |    |    |    |    |
+----+----+----+----+----+
|5   |6   |7   |8   |9   |
|    |    |    |    |    |
+----+----+----+----+----+
|10  |11  |12  |13  |14  |
|    |    |    |    |    |
+----+----+----+----+----+
|15  |16  |17  |18  |19  |
|    |    |    |    |    |
+----+----+----+----+----+
```
La pared al ESTE de la celda 4, por ejemplo, es siempre 1 porque 4%5 == 4 (comprobación que queremos eliminar). 
Pero es que la casilla al ESTE de la celda 4 se corresponde con la casilla 5 de forma aritmética (
la casilla ESTE de una casilla se calcula sumando 1). Y esa casilla, al ser la primera de la columna,
se le inicializó el valor de su pared OESTE a 1: claramente las comprobaciones en las casillas 4, 9 y 14 funcionan perfectamente.

Para la celda 19, su (casilla + CASILLA ESTE) es la 20, que no se inicializó. Si ponemos esta casilla
20 con su paredO a 1, podemos eliminar el cálculo del módulo en el código del principio del post y
todo funciona perfectamente.

Para las celdas SUR, ocurre algo parecido: la celda (15 +CASILLA SUR) y siguientes de la fila no
están inicializadas. Así que, de forma análoga, inicializamos una fila dummy entera: la primera casilla
con su paredO, y todas con su paredN a 1.

Lo importante es que no estamos realmente desperdiciando memoria (siempre tenemos nuestro array
de 16x16) y seguimos manteniendo los índices de casilla en un número de 8 bits.

# Laberinto de 16x16

En el caso del laberinto de 16x16, los índices son así:

```
+----+----+  .  .  +----+
|0   |1   |  .  .  |15  |
|    |    |  .  .  |    |
+----+----+  .  .  +----+
|16  |17  |  .  .  |31  |
|    |    |  .  .  |    |
+----+----+  .  .  +----+
 .  .  .  .  .  .  .  .

+----+----+  .  .  +----+
|224 |16  |  .  .  |239 |
|    |    |  .  .  |    |
+----+----+  .  .  +----+
|240 |241 |  .  .  |255 |
|    |    |  .  .  |    |
+----+----+  .  .  +----+
```
Las casillas más al ESTE de todas las filas excepto la última siguen la misma regla anterior. Podemos
consultar su (casilla + CASILLA ESTE) que nos dará la paredO de la primera casilla de la siguente fila
(que siempre es 1). Y con la casilla 255, ocurre que al sumar 1 (casilla + CASILLA_ESTE) al entero de
8 bits se convierte en 0.

En el caso de las casillas al sur de la siguiente fila, también funciona, ya que al ser las casillas
enteros de 8 bits, se utiliza aritmética de módulo 256 y cada una de esas (casillas + CASILLA_SUR) se
corresponde con la casilla de la primera fila y la misma columna. Ej:
```
casilla = 241
casilla + casilla_SUR = 241 + 16 = 257
257 % 256 = 1
```

Resumiendo: podemos eliminar todas las comparaciones de módulo, simplemente inicializando una última
fila dummy en el caso de que las filas sean menores que 16.

```cpp
void laberinto_inicializa_valores() {                                                                                                                                                                  
    int idx = 0;                                                                                                                                                                                       
    for (idx = 0; idx < num_columnas * num_filas; idx++) {                                                                                                                                             
        celda[idx].paredN = idx < num_columnas;                                                                                                                                                        
        celda[idx].paredO = idx % num_columnas == 0;                                                                                                                                                   
        celda[idx].visitada = 0;                                                                                                                                                                       
    }                                                                                                                                                                                                  
                                                                                                                                                                                                       
    // Anadir fila dummy                                                                                                                                                                               
    if (num_filas < MAX_FILAS) {                                                                                                                                                                       
        celda[filas*columnas].paredO = 1;                                                                                                                                                              
        for (idx = 0; idx < num_columnas; idx++) {                                                                                                                                                     
            celda[idx + filas*columnas].paredN = 1;                                                                                                                                                    
        }                                                                                                                                                                                              
    }                                                                                                                                                                                                  
}                                                                                                                                                                                                      
```

Ojo que ahora la constante MAX_FILAS y MAX_COLUMNAS es más constante que nunca. Antes podríamos
poner cualquier valor (siempre que hubiera memoria disponible

