---
layout: post
title:  "Terminando con accion y robot. Y Makefile"
commit: c158f688ae44fde1948b3e00d70c7e3b041f8ea8
---

Terminamos de documentar el código de `accion.cpp` y `robot.cpp`. Finalmente
ha aparecido algún error que provocaba un mapeo erróneo del laberinto.

- Al empezar la primera acción de arranque ya se incrementaba una casilla la cuenta de la
posición, y a veces se sincronizaba correctamente al desaparecer la pared
derecha, pero a veces no. Se soluciona poniendo como primer estado el de _PAUSA INICIAL_.

- Hemos tenido que añadir una función `accion_interrumpe()` que cancela la acción actual.
No quisimos dedicarle más tiempo ahora porque lo que queremos es documentar lo que hay,
pero cuando nos encontrábamos con una pared que no habíamos detectado al entrar en la
casilla, también se controlaba mal el cambio de posición. Por ahora, parece que se soluciona
poniendo también la velocidad final a 0, para que la función `accion_cambio_casilla()` no
interprete que efectivamente ha habido un cambio.

# Fichero Makefile

Estábamos acostumbrados a compilar con `pio run` en otra ventana, pero ya que usamos _vim_
para compilar, hacemos un fichero Makefile para que funcione la instrucción `make` y podamos
invocarla desde _vim_ y utilizar la ventana de `quickfix` para movernos rápidamente por los
errores de compilación. Aprovechamos para meter unos cuando comandos que estamos utilizando
continuamente.

```bash
PIO := pio run
LOGFILE := /tmp/datos.dat
BTADDR := 98:D3:32:10:90:95
PORT := /dev/rfcomm0

default:
	$(PIO)

upload:
	$(PIO) --target upload --upload-port=/dev/ttyUSB0

monitor:
	pio device monitor --port /dev/ttyUSB0

$(PORT):
	@echo "No existe port. Creando bind"
	sudo rfcomm bind /dev/rfcomm0 $(PORT)

monitortee: $(PORT)
	pio device monitor --port /dev/rfcomm0 | tee $(LOGFILE)

doc:
	doxygen
```
