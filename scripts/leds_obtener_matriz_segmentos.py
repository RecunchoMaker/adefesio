#!/usr/bin/env python

import time
import argparse
import sys

#!/usr/bin/env python
import sys
import argparse

def parse_options():
    usage = "Este programa calcula los segmentos en los que guardar " \
            "la curva caracteristica de los sensores."

    parser = argparse.ArgumentParser(description=usage)

    parser.add_argument(
        '-i', '--fichero',
        help='Fichero de entrada',
        default='/tmp/sensores-todos.dat')

    parser.add_argument(
        '-b', '--bits',
        type = int,
        help='numero de bits',
        default=6)

    args = parser.parse_args()

    return args

args = parse_options()

fichero = open(args.fichero, 'r')
s = [0] * 5
valores = []
for linea in fichero:
    # lee milimetros y valores de sensores
    mm,s[0],s[1],s[2],s[3] = map(int,linea.strip().split('\t'))

    media = (s[0] + s[1] + s[2] + s[3]) / 4

    valores.append(media)

corte = 2**args.bits
segmentos = []
segmentos.append([0,199])
for i in range(len(valores)):
    if valores[i] > corte:
        segmentos.append([corte,199-i])
        corte = corte + 2**args.bits
    if valores[i] > corte:
        segmentos.append([corte,199-i])
        corte = corte + 2**args.bits
segmentos.append([1023,0])

for i in segmentos:
    print i
print "len = ", len(segmentos)
def interpola(lectura):
    i = (lectura >> args.bits)
    pendiente = (1.0*segmentos[i+1][1] - segmentos[i][1]) / (2**args.bits)
    print segmentos[i][0]
    print "interp de %d = %f " % (lectura, segmentos[i][1] + pendiente * ( lectura - segmentos[i][0]))

# algunas pruebas
interpola(32)
interpola(100)
interpola(200)
interpola(969)


fichero.close()

print ("// Constantes generadas por leds_obtener_matriz_segmentos.py")
print "const uint8_t leds_segmentos[%d] = { \ " % (2**(10-args.bits) + 1)
for i in range(len(segmentos)):
    if i%8 == 0:
        print "    ",
    print segmentos[i][1],
    if i < len(segmentos)-1:
        print ",",
    else:
        print "};";
    if i%8 == 7 and i < len(segmentos) - 1:
        print " \ "

