---
layout: post
title:  "Flujo de trabajo con el intérprete de comandos"
---

Ahora que tenemos el intérprete listo, y para poder hacer montones
de pruebas rápidas, lanzamos el monitor (con la opción `echo`) y vamos
probando distintas constantes de pid simplemente tecleando a través del
puerto serie:

```
~/git/adefesio_master$ [master *%=] pio device monitor --echo | tee docs/plots/2019-01-27-con-prompt-1.dat
--- Miniterm on /dev/ttyUSB0  115200,8,N,1 ---
--- Quit: Ctrl+C | Menu: Ctrl+T | Help: Ctrl+T followed by Ctrl+H ---
A> kp -1.0
kp=-1.00
A> go
GO!
ultimaVel velObjetivo errorLeft errorAcumulado kp kd ki potencia ticks
0.00000 0.10000 -0.10000 0.00000 0.10000 0.00000 0.00000 0.10000 146
(...)
0.00000 0.10000 -0.10000 -0.10000 0.10000 0.00000 0.00000 0.20000 147
0.06085 0.10000 -0.03915 -0.03437 0.03915 0.00000 0.00000 0.50691 0
0.07085 0.10000 -0.02915 -0.03915 0.02915 0.00000 0.00000 0.53606 0
A> ki -0.1
ki=-0.10
A> go
GO!
ultimaVel velObjetivo errorLeft errorAcumulado kp kd ki potencia ticks
0.00000 0.10000 -0.10000 -0.00138 0.10000 0.00000 0.00014 0.10014 180
0.00000 0.10000 -0.10000 -0.10000 0.10000 0.00000 0.01000 0.21014 181
0.00000 0.10000 -0.10000 -0.10000 0.10000 0.00000 0.01000 0.32014 182
0.00000 0.10000 -0.10000 -0.10000 0.10000 0.00000 0.01000 0.65014 185
(...)
0.05324 0.10000 -0.04676 -0.03726 0.04676 0.00000 0.00373 0.50159 0
0.06406 0.10000 -0.03594 -0.04676 0.03594 0.00000 0.00468 0.54220 0
0.07574 0.10000 -0.02426 -0.03594 0.02426 0.00000 0.00359 0.57005 0
0.10097 0.10000 0.00097 -0.02426 -0.00097 0.00000 0.00243 0.57150 1

A>

```

y así sucesivamente. En otra ventana lanzamos un pequeño _oneliner_ en bash
que nos extrae del fichero de datos las últimas líneas desde el último `go`, 
y lanza el gnuplot.

```
while [ 1 ]; do tac 2019-01-27-con-prompt-1.dat  | sed '/^GO.*/,$ d' | tac | head -n -1 > /tmp/plot.dat | gnuplot 2019-01-27.gp; done
```

Para hacer las pruebas, sólo tenemos que:

- Cambiar una o varias constantes del pid
- Lanzar el comando _go_
- Ir a la ventana del gráfico de plot y pulsar una tecla para refrescar.

# Encontrando nuevos parámetros de PID.

Haciendo las pruebas tal y como indicamos antes es muy cómodo y rápido ir probando
valores. Encontramos que unos valores aceptables son:

```cpp
kp = -0.6
kd = -0.12
ki = -0.01
```
