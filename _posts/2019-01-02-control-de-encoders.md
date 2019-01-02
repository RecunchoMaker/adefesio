---
layout: post
title:  "Control de los encoders"
---
Vamos a conectar los encoders a nuestra placa y programar las funciones programadas para leerlos.

Cada uno de nuestros motores tiene 6 cables que vienen rotulados con Vcc, GND, C1, C2, I1 y I2. Primeramente,
para facilitar la conexión y desconexión, vamos a poner en cada uno de estos cables unos terminales Dupont hembra y
soldaremos en la placa dos parejas de pines macho (para conectar los motores) y una fila de 8 pines tambien 
macho para conectar los cables Vcc, GND, C1 y C2 de cada motor. Los pines correspondientes a I2 y I2 se
conectarán al puente H, según la tabla del post "Motores y puente H", y los demás, que son
los necesarios para los encoders, se conectan de la siguiente manera:

## Conexión al arduino

Los pines correspondientes a la alimentación de los motores irán al puente H

| arduino       | puente H  | Motor Izq | Motor Der |
|:--------------|:----------|:----------|:----------|
| VCC           | VCC,STBY  |    Vcc    |    Vcc    |
| GND           | GND       |    GND    |    GND    |
| D2            |           |    C1     |           |
| D12           |           |    C2     |           |
| D3            |           |           |    C1     |
| D8            |           |           |    C2     |

## Lectura de encoders

Los pines D2 y D3 del arduino están conectados a las interrupciones externas INT0 e INT1 del procesador.
Utilizaremos estas interrupciones para detectar los cambios de las entradas C1 de los encoders, lanzando
una función de interrupción que leerá la otra entrada del encoder (C2) para averiguar el sentido de giro
y actualizar las variables que correspondan. En principio, vamos a configurar estas interrupciones para
 que se disparen tanto en flanco de subida como de bajada, para aprovechar el máximo la resolución del encoder.

Creamos los ficheros _encoders.h_ y _encoders.cpp_ con las siguientes funciones:

```
void encoders_init(void);
void encoders_reset_posicion(void);
int16_t encoders_get_posicion_left(void);
int16_t encoders_get_posicion_right(void);
void encoders_ISR_left(void);
void encoders_ISR_right(void);
```

Mantendremos unas variables _volatiles_ (encoder_posicion_left y encoder_posicion_right) de tipo entero 16bits,
que contendrán la posición de los encoders, y crearemos unas funciones auxiliares para resetearlas y para
consultar su valor.

## Primeras pruebas

Preparamos temporalmente en el loop principal una pequeña prueba para volcar el valor de los encoders:

```cpp
void loop() {
    Serial.print(encoders_get_posicion_left());
    Serial.print(" ");
    Serial.print(encoders_get_posicion_right());
    Serial.println(" ");
    delay(100);
}
```

Sólo queda subir el sketch y mover manualmente las ruedas para comprobar que todas las conexiones funcionan
y los giros se detectan en el sentido correcto. Es importante que si la rueda va hacia adelante el incremento
sea positivo y si va hacia atrás en negativo. Aprovechamos también para ver cuántos pasos se capturan en
una vuelta completa de rueda, lo que nos da 716 pasos (un valor un tanto extraño).

## Una pequena prueba de movimiento

```
void loop() {
    encoders_reset_posicion();
    Serial.print("Avanzando durante 5 segundos");
    motor_set_pwm(100, 100);
    delay(5000);
    Serial.print(encoders_get_posicion_left());
    Serial.print(" ");
    Serial.print(encoders_get_posicion_right());
    Serial.println(" ");
    delay(5000);
}
```

Conectamos el bluetooth, hacemos el bind a rfcomm, lanzamos el monitor y encendemos el robot.

```
~/$ pio device monitor --port /dev/rfcomm0
--- Miniterm on /dev/rfcomm0  115200,8,N,1 ---       
--- Quit: Ctrl+C | Menu: Ctrl+T | Help: Ctrl+T followed by Ctrl+H --- 
Bateria: 7.66v                                       
Avanzando durante 5 segundos                         
7460 8100                                            
Avanzando durante 5 segundos                         
7450 8116                                            
Avanzando durante 5 segundos                         
7497 8141                                            
```

El robot empieza a moverse. Vemos por los valores finales del encoder (y también se nota bastante visualmente)
 que el robot no avanza en línea recta, sino que hace una pequeña curva. Hay varias causas que
pueden provocar eso: distintos rozamientos de cada motor, una pequeña variación en el radio de la rueda... de
todos modos, no nos preocupamos por ahora porque el objetivo será controlar el motor atendiendo a su
velocidad, ya que un valor de pwm no se corresponde exactamente con la velocidad obtenida y tendremos que
irlo actualizando con un PID.

_commit: 55a57a5f09d6af8461ddbbc051d8e4c74f51c2d3_
