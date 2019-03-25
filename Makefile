PIO := pio run
LOGFILE := /tmp/datos.dat
BTADDR := 98:D3:34:90:B6:1D
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

$(BTPORT):
	@echo "No existe port. Creando bind"
	sudo rfcomm bind $(BTPORT) $(BTADDR)
	sleep 1

monitortee: $(BTPORT)
	pio device monitor --port $(BTPORT) | tee $(LOGFILE)

doc:
	doxygen
