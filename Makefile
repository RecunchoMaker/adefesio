PIO := pio run
LOGFILE := /tmp/datos.dat
BTADDR := 98:D3:32:10:90:95
BTPORT := /dev/rfcomm0
USBPORT := /dev/ttyUSB0

default:
	$(PIO)

upload:
	$(PIO) --target upload --upload-port=$(USBPORT)

uploadbt:
	$(PIO) --target upload --upload-port=$(PORT)

monitor:
	pio device monitor --port $(USBPORT)

$(PORT):
	@echo "No existe port. Creando bind"
	sudo rfcomm bind /dev/rfcomm0 $(BTADDR)
	sleep 1

monitortee: $(PORT)
	pio device monitor --port /dev/rfcomm0 | tee $(LOGFILE)

doc:
	doxygen
