#!/usr/bin/env python
import sys
import argparse

def parse_options():
    usage = "Este programa separa las acciones de " \
            "log_variables_trayectoria() del robot Adefesio"

    parser = argparse.ArgumentParser(description=usage)

    parser.add_argument(
        '-q', '--quiet',
        action='store_false',
        help='no muestra mensajes de info por stderr',
        default=False)

    parser.add_argument(
        'FICHERO',
        nargs='?',
        help='fichero a parsear, por defecto: %(default)s',
        default='/tmp/datos.dat')


    args = parser.parse_args()

    return args


""" Devuelve True si una linea tiene el numero de elementos correctos """
def es_linea_correcta(linea):

    linea = linea.rstrip('\r').rstrip('\n')

    # Es un log?
    linea = linea.split(' ')

    if len(linea) <> 2:
        return False

    if linea[0] <> '#3':
        return False

    # Tiene el numero de valores correcto?
    linea = linea[1].split('\t')
    if len(linea) <> 10:
        return False

    # Son todos los valores float?
    try:
        linea = [int(linea[0]),
                 int(linea[1]),
                 float(linea[2]),
                 float(linea[3]),
                 float(linea[4]),
                 float(linea[5]),
                 float(linea[6]),
                 float(linea[7]),
                 float(linea[8]),
                 float(linea[9])]
    except:
        return False

    return True


""" Formatea una linea correcta como lista """
def formatea(linea):
    l = linea.split(" ")[1].split('\t')
    return [int(l[0]),
            int(l[1]),
            float(l[2]),
            float(l[3]),
            float(l[4]),
            float(l[5]),
            float(l[6]),
            float(l[7]),
            float(l[8]),
            float(l[9])]


""" Devuelve una lista con las lineas correctas del fichero de entrada """
def extraer_lineas_correctas(fichero):
    lineas = []
    with open(fichero, 'r') as f:
        #l = f.readline().rstrip('\n').rstrip('\r')
        l = f.readline()
        while l:
            if es_linea_correcta(l):
                lineas.append(formatea(l))
            l = f.readline()

    return lineas


""" Devuelve una accion en lista formateada como el log original """
def log(a):
    print "%d\t%d\t%f\t%f\t%f\t%f\t%d\t%d\t%d\t%d" % (a[0],a[1],a[2],a[3],a[4],a[5],a[6],a[7],a[8],a[9])


def main():
    args = parse_options()

    lineas = extraer_lineas_correctas(args.FICHERO)
    if not args.quiet:
        sys.stderr.write("%d lineas correctas en '%s'\n" %
                         (len(lineas), args.FICHERO))

    last_paso = 9999
    minimos = [0.20] * 4
    pasos = [0] * 4
    x = 0
    if lineas:
        for a in lineas:
            if a[1] < last_paso:
                if last_paso < 9999:
                    print "0 %d %f" % (pasos[0]+x,minimos[0])
                    print "1 %d %f" % (pasos[1]+x,minimos[1])
                    print "2 %d %f" % (pasos[2]+x,minimos[2])
                    print "3 %d %f" % (pasos[3]+x,minimos[3])

                    print "5 %d %f" % ( (x+ ((pasos[1]+pasos[2]) / 2)), (+minimos[1] + minimos[2]) / 2.0)
                    x = x + last_paso
                    print "4 %d %d" % (x,0)
                    print "4 %d %f" % (x+1,0.2)

                    minimos = [0.20] * 4
                    #print("nuevo paso a %d, %d %d" % (x, last_paso, a[1]))
            else:

                if minimos[0] > a[2]:
                    minimos[0] = a[2]
                    pasos[0] = a[1]
                if minimos[1] > a[3]:
                    minimos[1] = a[3]
                    pasos[1] = a[1]
                if minimos[2] > a[4]:
                    minimos[2] = a[4]
                    pasos[2] = a[1]
                if minimos[3] > a[5]:
                    minimos[3] = a[5]
                    pasos[3] = a[1]

            last_paso = a[1]

                #log (a)
        if not args.quiet:
            sys.stderr.write("%d acciones en '%s'\n" %
                             (len(lineas),args.FICHERO))


if __name__ == "__main__":
    main()
