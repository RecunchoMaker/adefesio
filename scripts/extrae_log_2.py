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
def es_linea_correcta(linea,empieza_con,tamano):

    linea = linea.rstrip('\r').rstrip('\n')

    # Es un log?
    linea = linea.split(' ')

    if len(linea) != 2:
        return False

    if linea[0] != empieza_con:
        return False

    # Tiene el numero de valores correcto?
    linea = linea[1].split('\t')
    if len(linea) != tamano:
        return False


    # Son todos los valores float?
    try:
        linea = map(float, linea)
    except:
        print("no mapea", linea)
        return False

    return True

""" Formatea una linea correcta como lista """
def formatea(linea):
    l = linea.split(" ")[1].split('\t')
    return map(float,l)

""" Devuelve una lista con las lineas correctas del fichero de entrada """
def extraer_lineas_correctas(fichero, empieza_con,tamano):
    lineas = []
    with open(fichero, 'r') as f:
        l = f.readline()
        #l = f.readline()
        while l:
            l = l.rstrip('\n').rstrip('\r')
            if es_linea_correcta(l,empieza_con,tamano):
                lineas.append(formatea(l))
            #l = f.readline()
            l = f.readline()

    return lineas


""" Devuelve una accion en lista formateada como el log original """
def log(a):
    print "%d\t%d\t" % (a[0],a[1]),
    for f in a[2:]:
        print "%0.5f"%f,
    print ""


def main():
    args = parse_options()

    lineas = extraer_lineas_correctas(args.FICHERO, '#2', 9)
    if not args.quiet:
        sys.stderr.write("%d lineas correctas en '%s'\n" %
                         (len(lineas), args.FICHERO))

    for l in lineas:
        log(l)


if __name__ == "__main__":
    main()
