#!/usr/bin/env python

import serial
import time
import argparse
import sys

#!/usr/bin/env python
import sys
import argparse

def parse_options():
    usage = "Este programa utiliza una impresora 3d para calibrar " \
            "los sensores del robot Adefesio"

    parser = argparse.ArgumentParser(description=usage)

    parser.add_argument(
        '-i', '--impresora',
        help='Puerto de la impresora',
        default='/dev/ttyUSB0')

    parser.add_argument(
        '-a', '--adefesio',
        help='Puerto de adefesio',
        default='/dev/rfcomm0')

    args = parser.parse_args()

    return args

def envia_gcode(gcode):
    impresora.write(gcode + '\n')  # home de los ejes X e Y
    respuesta = impresora.readline()
    while respuesta.strip() <> 'ok':
        respuesta = impresora.readline()

def es_linea_correcta(linea):

    linea = linea.rstrip('\r').rstrip('\n')

    # Es un log?
    linea = linea.split(' ')

    if len(linea) <> 2:
        return False

    if linea[0] <> '#1':
        return False

    # Tiene el numero de valores correcto?
    linea = linea[1].split('\t')
    if len(linea) <> 8:
        return False

    # Son todos los valores enteros?
    try:
        linea = [int(linea[0]),
                 int(linea[1]),
                 int(linea[2]),
                 int(linea[3]),
                 int(linea[4]),
                 int(linea[5]),
                 int(linea[6]),
                 int(linea[7])]
    except:
        return False

    return True


""" Formatea una linea correcta como lista """
def formatea(linea):
    l = linea.split(" ")[1].split('\t')
    return [int(l[0]),
            int(l[1]),
            int(l[2]),
            int(l[3]),
            int(l[4]),
            int(l[5]),
            int(l[6]),
            int(l[7])]



args = parse_options()

impresora = serial.Serial(args.impresora,115200)

time.sleep(2)

print "Conectando a Adefesio..."
sys.stdout.flush()
adefesio = serial.Serial(args.adefesio,115200)
prompt = adefesio.read(2)
while prompt <> 'A>':
    adefesio.readline()
    sys.stdout.flush()
    prompt = adefesio.read(2)
adefesio.readline()


print "Homming..."
sys.stdout.flush()
envia_gcode('G28') # home
envia_gcode('G91') # pasamos a movimiento relativo
envia_gcode('M201 Y50') # aceleracion lenta


adefesio.write('go\r\n')

x = 0
while x < 200:
    envia_gcode('G1 Y1') # Mueve y 1 milimetro
    sensores = adefesio.readline()
    while not es_linea_correcta(sensores):
        sensores = adefesio.readline()

    log = formatea(sensores)
    print "%d\t%d\t%d\t%d\t%d\t" % (x,
                                    log[2],log[3],log[4],log[5])
    sys.stdout.flush()
    x = x + 1

envia_gcode('M18') # apaga motores

impresora.close()

