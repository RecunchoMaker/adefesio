import API
import sys
import serial
import time


filas = 3
columnas = 5

def log(string):
    sys.stderr.write("{}\n".format(string))


def casilla2x(casilla):
    global filas, columnas
    return casilla % columnas;

def casilla2y(casilla):
    global filas, columnas
    return filas - 1 - casilla / columnas;

def main():
    global filas, columnas
    arduino = serial.Serial()
    arduino.baudrate = 115200
    arduino.timeout = 1
    arduino.port = '/dev/ttyUSB0'
    arduino.open()

    a = 0

    response = ''
    while True:
        response += arduino.read()
        if 'A>' in response:
            break

    time.sleep(1)

    arduino.write('lf 1\r'.encode())
    time.sleep(1)

    arduino.write('go\r'.encode())
    log("go!")

    # picocom -b 19200 --imap lfcr --omap crcrlf /dev/ttyUSB0


    while True:
        linea = arduino.readline().strip()
        """
        response = ''
        while True:
            response += arduino.read()
            if len(response) > 10:
                log(response)
                linea = response
                break
        """

        arduino.write('lf %s' % (API.wallRight()))
        if (linea):
            try:
                if "f?" in linea:
                    log("frontal?")
                    if API.wallFront():
                        arduino.write("1");
                    else:
                        arduino.write("0");
                if "i?" in linea:
                    log("izquierdo?")
                    if API.wallLeft():
                        arduino.write("1");
                    else:
                        arduino.write("0");
                if "lder?" in linea:
                    log("derecho?")
                    if API.wallRight():
                        arduino.write("1");
                    else:
                        arduino.write("0");
                elif linea == "moveForward":
                    API.moveForward()
                elif linea == "turnLeft":
                    API.turnLeft()
                elif linea == "turnRight":
                    API.turnRight()
                elif linea == "sd":
                    sensor = API.wallRight()
                    log("sensor derecha: %s" % sensor)
                elif linea == "si":
                    sensor = API.wallLeft()
                    log("sensor izquierda: %s" % sensor)
                elif linea == "sf":
                    sensor = API.wallFront()
                    log("sensor frontal: %s" % sensor)
                elif "setParedLateral" in linea:
                    casilla,ori,izq,der = map(int,linea.split(" ")[1:])
                    if (der):
                        log("setWall %d %d %s" % (casilla2x(casilla), casilla2y(casilla),"neswnesw"[ori+1]))
                        API.setWall(casilla2x(casilla),casilla2y(casilla),"neswnesw"[ori+1])
                    if (izq):
                        log("setWall %d %d %s" % (casilla2x(casilla), casilla2y(casilla),"neswnesw"[ori+3]))
                        API.setWall(casilla2x(casilla),casilla2y(casilla),"neswnesw"[ori+3])
                elif "camino: " in linea:
                    log(linea)
                    camino = map(int,linea.split(" ")[1:])
                    for p in camino:
                        API.setColor(casilla2x(p),casilla2y(p),"G")
                else:
                    log(linea)
                time.sleep(0.2)
            except Exception as inst:
                log ("Error: ")
                log(type(inst))    # the exception instance
                log(inst.args)     # arguments stored in .args
                log(inst)          # __str__ allows args to be printed directly,
        else:
            log(linea);










    """
    if (API.wallFront()):
        maze[x][y][orientacion] = 1
    if (API.wallLeft()):
        maze[x][y][(orientacion+1) % 4] = 1
    if (API.wallRight()):
        maze[x][y][(orientacion-1) % 4] = 1

        API.moveForward()
    """

if __name__ == "__main__":
    main()
