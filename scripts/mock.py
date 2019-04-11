import API
import sys
import serial
import time


filas = 9
columnas = 9

def log(string):
    sys.stderr.write("{}\n".format(string))


def casilla2x(casilla):
    global filas, columnas
    return casilla % columnas;

def casilla2y(casilla):
    global filas, columnas
    return filas - 1 - casilla / columnas;

def enviaSensores(arduino):
    res = ""
    if (API.wallFront()):
        arduino.write('lf 1\r'.encode())
        res = res + "F"
    else:
        arduino.write('lf 0\r'.encode())
        res = res + " "
    #time.sleep(0.01)

    if (API.wallRight()):
        arduino.write('ld 1\r'.encode())
        res = res + "D"
    else:
        arduino.write('ld 0\r'.encode())
        res = res + " "
    #time.sleep(0.01)

    if (API.wallLeft()):
        res = res + "I"
        arduino.write('li 1\r'.encode())
    else:
        res = res + " "
        arduino.write('li 0\r'.encode())
    #time.sleep(0.01)
    log("Enviados sensores: %s" % res)


def main():
    global filas, columnas
    arduino = serial.Serial()
    arduino.baudrate = 115200
    arduino.timeout = 0.1
    arduino.port = '/dev/ttyUSB0'
    arduino.open()

    a = 0

    response = ''
    while True:
        response += arduino.read()
        if 'A>' in response:
            break

    enviaSensores(arduino)

    # picocom -b 19200 --imap lfcr --omap crcrlf /dev/ttyUSB0
    i = 0

    camino = []

    while True:
        time.sleep(0.02)
        i = i + 1
        linea = arduino.readline().strip()


        if i > 16:
            i = 0
            log("fuerzo siguiente accion");
            arduino.write("sa\r".encode())

        if (linea):
            try:
                if linea == "moveForward":
                    API.moveForward()
                    enviaSensores(arduino)
                    arduino.write("sa\r".encode())
                    i = 0
                elif linea == "turnLeft":
                    API.turnLeft()
                    enviaSensores(arduino)
                    #arduino.write("sa\r".encode())
                    i = 0
                elif linea == "turnRight":
                    API.turnRight()
                    enviaSensores(arduino)
                    #arduino.write("sa\r".encode())
                    i = 0
                elif linea == "turn180":
                    API.turnLeft()
                    enviaSensores(arduino)
                    API.turnLeft()
                    enviaSensores(arduino)
                    #arduino.write("sa\r".encode())
                    i = 0
                elif "setParedLateral" in linea:
                    casilla,ori,izq,der = map(int,linea.split(" ")[1:])
                    if (der):
                        log("setWall %d %d %s" % (casilla2x(casilla), casilla2y(casilla),"neswnesw"[ori+1]))
                        API.setWall(casilla2x(casilla),casilla2y(casilla),"neswnesw"[ori+1])
                    if (izq):
                        log("setWall %d %d %s" % (casilla2x(casilla), casilla2y(casilla),"neswnesw"[ori+3]))
                        API.setWall(casilla2x(casilla),casilla2y(casilla),"neswnesw"[ori+3])
                elif "setParedFrontal" in linea:
                    log("pared frontal: %s" % linea)
                    casilla,ori,frente = map(int,linea.split(" ")[1:])
                    if (frente):
                        log("setWall %d %d %s" % (casilla2x(casilla), casilla2y(casilla),"nesw"[ori]))
                        API.setWall(casilla2x(casilla),casilla2y(casilla),"nesw"[ori])
                elif "flood" in linea:
                    casilla,peso= map(int,linea.split(" ")[1:])
                    #log("setWall %d %d %s" % (casilla2x(casilla), casilla2y(casilla),peso))
                    API.setText(casilla2x(casilla),casilla2y(casilla),str(peso))
                elif "camino: " in linea:
                    log(linea)
                    for p in camino:
                        API.setColor(casilla2x(p),casilla2y(p),"k")
                    camino = map(int,linea.split(" ")[1:])
                    for p in camino:
                        API.setColor(casilla2x(p),casilla2y(p),"G")
                    #arduino.write("sa\r".encode())
                elif "nextAction" in linea:
                    log("pide siguiente")
                    arduino.write("sa\r".encode())
                else:
                    log(linea)
                    pass
            except Exception as inst:
                log ("Error: ")
                log(type(inst))    # the exception instance
                log(inst.args)     # arguments stored in .args
                log(inst)          # __str__ allows args to be printed directly,
        else:
            pass
            #log(linea);


if __name__ == "__main__":
    main()
