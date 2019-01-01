---
layout: post
title:  "Conexión bluetooth desde la consola"
---
Nos resulta un poco pesado que cada vez que reiniciamos el robot se pierde
el dispositivo rfcomm y tenemos que utilizar la interface gráfica para recrearlo.
Esto genera un nuevo dispositivo (rfcomm1, rfcomm2... etc.) y es bastante
incómodo.

Vamos a averiguar cómo emparejar nuestro módulo mediante la línea de comandos, pensando en futuros scripts que automaticen
la conexión.

Utilizamos la utilidad _bluetoothctl_. Al ejecutarla
se nos abre un prompt en el que veremos nuestros dispositivos emparegados, 
podemos escanear los nuevos, y emparejarlos (se nos solicitará el pin). El proceso sería
el siguiente:

1. Encendemos el robot con el bluetooth conectado. El módulo parpadeará rápidamente
indicando que está en modo discovery y que no está emparejado.

2. Ejecutamos bluetoothctl y emparejamos
```
~$ bluetoothctl                                      
[NEW] Controller 60:57:18:39:FA:11 slim [default]    
[NEW] Device 20:16:11:23:98:34 HC-06           
Agent registered                                     
[bluetooth]# scan on                                 
Discovery started                                    
[CHG] Controller 60:57:18:39:FA:11 Discovering: yes  
(...)
[NEW] Device 98:D3:32:10:90:95 HC-05
(...)
[bluetooth]# pair 98:D3:32:10:90:95                  
Attempting to pair with 98:D3:32:10:90:95            
[CHG] Device 98:D3:32:10:90:95 Connected: yes        
Request PIN code                                     
[agent] Enter PIN code: 0000                         
Pairing successful                                   
[BTMICHI1]# quit                                     
Agent unregistered                                   
~$                                                   
```

3. El módulo emparejado empezará hará dos parpadeos rápidos cada dos segundos
aproximadamente, indicando que está emparejado.

4. Crear dipositivo RFCOMM
Una vez emparejado, necesitamos crear un bind entre un dispositivo rfcomm (que
crearemos en /dev/rfcomm0) y nuestro módulo bluetooth.

```
$ sudo rfcomm bind /dev/rfcomm0 98:D3:32:10:90:95
```

Y listo. Ya podemos ejecutar 'pio device monitor --port /dev/rfcomm0', manteniéndose
el emparejamiento incluso después de apagar el robot. 

Después de reinciciar el ordenador, no hace falta volver a emparejar con bluetoothctl,
pero SI crear el bind con el comando rfcom. Curiosamente, el módulo sigue parpadeando
como si estuviera desemparejado pero al solicitar datos por el puerto serie se empareja de nuevo correctamente.
