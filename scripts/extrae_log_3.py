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
    print (len(linea))
    if len(linea) <> 6:
        return False

    # Son todos los valores float?
    try:
        linea = [int(linea[0]),
                 int(linea[1]),
                 float(linea[2]),
                 float(linea[3]),
                 float(linea[4]),
                 float(linea[5])]
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
            float(l[5])]


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
    print "%d\t%d\t%f\t%f\t%f\t%f" % (a[0],a[1],a[2],a[3],a[4],a[5])


def main():
    args = parse_options()

    lineas = extraer_lineas_correctas(args.FICHERO)
    if not args.quiet:
        sys.stderr.write("%d lineas correctas en '%s'\n" %
                         (len(lineas), args.FICHERO))

    if lineas:
        for a in lineas:
            log (a)
        if not args.quiet:
            sys.stderr.write("%d acciones en '%s'\n" %
                             (len(lineas),args.FICHERO))


if __name__ == "__main__":
    main()
