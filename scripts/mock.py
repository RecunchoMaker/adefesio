import API
import sys
import serial
import time



def log(string):
    sys.stderr.write("{}\n".format(string))



class Arduino:

    def __init__(self, port, speed):
        self.ser = serial.Serial()
        self.ser.baudrate = speed
        self.ser.timeout = 0.1
        self.ser.port = port
        self.ser.open()

    def lee(self):
        return self.ser.readline().strip()

    def espera_por(self, palabra):
        # log("espero por '%s'" % palabra)
        while True:
            linea = self.lee()
            if palabra in linea:
                # log("recibido: %s" % linea)
                break

    def envia(self, linea):
        self.ser.write(linea + '\r');

    def envia_sensores(self):
        res = ""

        if (API.wallRight()):
            self.ser.write('ld 1\r'.encode())
            res = res + "D"
        else:
            self.ser.write('ld 0\r'.encode())
            res = res + " "
        if (API.wallLeft()):
            res = res + "I"
            self.ser.write('li 1\r'.encode())
        else:
            res = res + " "
            self.ser.write('li 0\r'.encode())
        if (API.wallFront()):
            self.envia('lf 1')
            res = res + "F"
        else:
            self.ser.write('lf 0\r'.encode())
            res = res + " "
        # log("Enviados sensores: %s" % res)
        self.espera_por("sensores IFD")
        # log("ack sensores - siguiente accion")
        self.envia("sa")

class Mock:

    def __init__(self, filas, columnas):
        self.filas = filas
        self.columnas = columnas
        self.camino = []

    def set_arduino(self, arduino):
        self.arduino = arduino

    def casilla2x(self,casilla):
        return casilla % self.columnas;

    def casilla2y(self,casilla):
        return self.filas - 1 - casilla / self.columnas;

    def interpreta_linea(self, linea):

        if not linea:
            return

        try:
            if "sensores IFD" in linea:
                log("ack sensores")
                self.arduino.envia("sa")
            if linea == "moveForward":
                API.moveForward()
                self.arduino.envia_sensores()
                i = 0
            elif linea == "turnLeft":
                API.turnLeft()
                self.arduino.envia_sensores()
                #arduino.write("sa\r".encode())
                i = 0
            elif linea == "turnRight":
                API.turnRight()
                self.arduino.envia_sensores()
                #arduino.write("sa\r".encode())
                i = 0
            elif linea == "turn180":
                API.turnLeft()
                API.turnLeft()
                self.arduino.envia_sensores()
                i = 0
            elif "setParedLateral" in linea:
                casilla,ori,izq,der = map(int,linea.split(" ")[1:])
                if (der):
                    # log("setWall %d %d %s" % (self.casilla2x(casilla), self.casilla2y(casilla),"neswnesw"[ori+1]))
                    API.setWall(self.casilla2x(casilla),self.casilla2y(casilla),"neswnesw"[ori+1])
                if (izq):
                    # log("setWall %d %d %s" % (self.casilla2x(casilla), self.casilla2y(casilla),"neswnesw"[ori+3]))
                    API.setWall(self.casilla2x(casilla),self.casilla2y(casilla),"neswnesw"[ori+3])
            elif "setParedFrontal" in linea:
                # log("pared frontal: %s" % linea)
                casilla,ori,frente = map(int,linea.split(" ")[1:])
                if (frente):
                    # log("setWall %d %d %s" % (self.casilla2x(casilla), self.casilla2y(casilla),"nesw"[ori]))
                    API.setWall(self.casilla2x(casilla),self.casilla2y(casilla),"nesw"[ori])
            elif "flood" in linea:
                casilla,peso= map(int,linea.split(" ")[1:])
                #log("setWall %d %d %s" % (self.casilla2x(casilla), self.casilla2y(casilla),peso))
                API.setText(self.casilla2x(casilla),self.casilla2y(casilla),str(peso))
            elif "camino: " in linea:
                # log(linea)
                for p in self.camino:
                    API.setColor(self.casilla2x(p),self.casilla2y(p),"k")
                self.camino = map(int,linea.split(" ")[1:])
                for p in self.camino:
                    API.setColor(self.casilla2x(p),self.casilla2y(p),"G")
                #arduino.write("sa\r".encode())
            elif "nextAction" in linea:
                self.arduino.envia('sa')
            else:
                log("> %s" % linea)
                pass
        except Exception as inst:
            log ("Error: '%s'" % linea)
            log(type(inst))    # the exception instance


def main():

    filas = 16
    columnas = 16

    mock = Mock(filas, columnas)
    arduino = Arduino('/dev/ttyUSB0',115200)
    mock.set_arduino(arduino)

    arduino.espera_por('A>');
    log ("Conexion establecida")


    arduino.envia('go')

    while True:
        mock.interpreta_linea(arduino.lee())


if __name__ == "__main__":
    main()
