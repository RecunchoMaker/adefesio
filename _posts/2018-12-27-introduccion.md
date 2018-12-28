---
layout: post
title:  "Preparando el entorno con platform.io"
---
Para la programación de arduino vamos a utilizar [platform.io](https://platformio.org). Aunque
dispone de entornos integrados de desarrollo (IDE) que no hemos probado, nosotros vamos a utilizar únicamente
el **PlatformIO Core**. Con esta herramienta, disponemos de un conjunto de comandos en la
terminal para compilar, subir el firmware al arduino, monitorizar el puerto serie... etc. Y
podremos utilizar nuestro editor de textos favorito. No hay más razón para esto que una
preferencia personal, aunque ciertamente el ArduinoIDE se queda muy corto en un proyecto
que incluya varios ficheros.

La instalación se realiza desde el gestor de paquetes de python (pip).

```console
pip install -U platformio
```

Una vez instalado, creamos una carpeta para nuestros ficheros, hacemos cd a la carpeta, e
inicializamos el proyecto platformio para el procesador que utilizemos, en este caso, un 
arduino nano.

```console
/home/user$ mkdir adefesio
/home/user$ cd adefesio
/home/user/adefesio$ pio boards | grep nano
nanoatmega168         ATMEGA168      16MHz     14KB    1KB    Arduino Nano ATmega168
nanoatmega328         ATMEGA328P     16MHz     30KB    2KB    Arduino Nano ATmega328
nanoatmega328new      ATMEGA328P     16MHz     30KB    2KB    Arduino Nano ATmega328 (New Bootloader)
nano32                ESP32          240MHz    4MB     320KB  MakerAsia Nano32
redbear_blenano2      NRF52832       64MHz     512KB   64KB   RedBearLab BLE Nano 2
/home/user/adefesio$ pio init --board nanoatmega328

The current working directory /home/user/adefesio will be used for the project.

The next files/directories have been created in /home/user/adefesio
include - Put project header files here
lib - Put here project specific (private) libraries
src - Put project source files here
platformio.ini - Project Configuration File

Project has been successfully initialized! Useful commands:
`pio run` - process/build project from the current directory
`pio run --target upload` or `pio run -t upload` - upload firmware to a target
`pio run --target clean` - clean project (remove compiled files)
`pio run --help` - additional information

/home/user/adefesio$  tree
.
├── include
│   └── README
├── lib
│   └── README
├── platformio.ini
├── src
└── test
    └── README

4 directories, 4 files
/home/user/adefesio$ 
```

Con el comando **pio init --board __board__** platformio crea una estructura de directorios básica en la 
que pondremos nuestro código fuente y librerías. Como prueba inicial, creamos un main.cpp en la
carpeta **src**

```cpp
#include <Arduino.h>

void setup() {
    Serial.begin(115200);
}

void loop() {
    Serial.println("Hola mundo");
    delay(1000);
}
```

Una vez creado el fichero, desde la carpeta raiz del proyecto, podemos compilar con 'pio run'. Si todo
va bien, contectamos el arduino y subiremos el firmware con 'pio run --target upload', y comprobamos
que efectivamente está ejecutando el loop escribiendo en el puerto serie.

```console
/home/user/adefesio$ pio run
Processing nanoatmega328 (platform: atmelavr; board: nanoatmega328; framework: arduino)
---------------------------------------------------------------------------------------
Verbose mode can be enabled via `-v, --verbose` option
CONFIGURATION: https://docs.platformio.org/page/boards/atmelavr/nanoatmega328.html
PLATFORM: Atmel AVR > Arduino Nano ATmega328
HARDWARE: ATMEGA328P 16MHz 2KB RAM (30KB Flash)
(...)
Memory Usage -> http://bit.ly/pio-memory-usage
DATA:    [=         ]   9.7% (used 198 bytes from 2048 bytes)
PROGRAM: [=         ]   5.3% (used 1640 bytes from 30720 bytes)
====================================================================

/home/user/adefesio$ pio run --target upload
(...)
avrdude: safemode: Fuses OK (E:00, H:00, L:00)
avrdude done.  Thank you.

/home/user/adefesio$ pio device monitor --baud 115200
--- Miniterm on /dev/ttyUSB0  115200,8,N,1 ---
--- Quit: Ctrl+C | Menu: Ctrl+T | Help: Ctrl+T followed by Ctrl+H ---
Hola mundo
Hola mundo
Hola mundo
```

> **tip**
> Para evitar tener que especificar la velocidad del puerto serie, anadimos la siguiente línea al fichero
> platformio.ini
>
> monitor_speed = 115200

_commit_: 986422e520c65bc79119adf60770a9d7ef00067d
