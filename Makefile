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
