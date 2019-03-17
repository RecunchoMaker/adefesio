#!/usr/bin/env python
import argparse

def parse_options():
    usage = "Este programa separa las acciones de " \
            "log_casillas_sensores() del robot Adefesio"

    parser = argparse.ArgumentParser(description=usage)

    parser.add_argument(
        '-r', '--recto',
        action='store_true',
        help='filtra accion RECTO',
        default=False)

    parser.add_argument(
        '-a', '--arrancar',
        action='store_true',
        help='filtra accion ARRANCAR',
        default=False)

    parser.add_argument(
        '-p', '--parar',
        action='store_true',
        help='filtra accion PARAR',
        default=False)

    parser.add_argument(
        '-g', '--giro',
        action='store_true',
        help='filtra accion GIRO',
        default=False)

    parser.add_argument(
        '-v', '--verbose',
        action='store_true',
        help='muestra ciertos mensajes de informacion',
        default=False)

    parser.add_argument(
        'FICHERO',
        nargs='?',
        help='fichero a parsear, por defecto: %(default)s',
        default='/tmp/data.dat')


    args = parser.parse_args()

    return args


""" Devuelve True si una linea tiene 6 elementos enteros en los margenes correctos"""
def es_linea_correcta(linea):

    v = linea.rstrip('\n').split("\t")
    if len(v) <> 6:
        return False

    try:
        v = map(int,v)
    except:
        return False

    return True


""" Formatea una linea correcta como lista """
def formatea(linea):
    return map(int, linea.rstrip('\n').split("\t"))


""" Devuelve una lista con las lineas correctas del fichero de entrada """
def extraer_lineas_correctas(fichero):
    lineas = []
    with open(fichero, 'r') as f:
        l = f.readline()
        while l:
            if es_linea_correcta(l):
                lineas.append(formatea(l))
            l = f.readline()

    return lineas


""" Extrae las distintas acciones de una lista de lineas """
def extraer_acciones(lista):
    acciones = []
    pasos_ultimos = lista[0][1]

    ultima_accion = []

    for l in lista:
        if l[1] >= pasos_ultimos:
            ultima_accion.append(l)
        else:
            acciones.append(ultima_accion);
            ultima_accion = []
        pasos_ultimos = l[1]

    acciones.append(ultima_accion);
    return acciones


""" Devuelve true si una accion tiene mas de 400 pasos """
def accion_es_recto(accion):
    max_pasos = 0
    for a in accion:
        max_pasos = max(max_pasos, a[1])
    return max_pasos > 400

""" Devuelve true si una accion tiene entre 200 y 250 pasos """
def accion_es_arrancar_parar(accion):
    max_pasos = 0
    for a in accion:
        max_pasos = max(max_pasos, a[1])
    return max_pasos < 250 and max_pasos > 200

""" Devuelve true si una accion es arrancar o parar, y al decimo log aun no avanzaron los pasos """
def accion_es_arrancar(accion):
    return accion_es_arrancar_parar(accion) and accion[10][1] == 0

""" Devuelve true si una accion es arrancar o parar, y al decimo ya avanzaron los pasos """
def accion_es_parar(accion):
    return accion_es_arrancar_parar(accion) and accion[10][1] > 0


""" Devuelve true si una accion tiene menos de 150 psaos """
def accion_es_giro(accion):
    max_pasos = 0
    for a in accion:
        max_pasos = max(max_pasos, a[1])
    return max_pasos < 150


""" Devuelve una accion en lista formateada como el log original """
def log(accion):
    for a in accion:
        print "%d\t%d\t%d\t%d\t%d\t%d" % (a[0],a[1],a[2],a[3],a[4],a[5])


def main():
    args = parse_options()

    lineas = extraer_lineas_correctas(args.FICHERO)
    if args.verbose:
        print(str(len(lineas)) + " lineas corrrectas en " + args.FICHERO)

    acciones = extraer_acciones(lineas)
    if args.verbose:
        print(str(len(acciones)) + " acciones en " + args.FICHERO)

    cuenta = 0
    for a in acciones:
        cuenta = cuenta + 1
        if accion_es_recto(a) and args.recto:
            log(a)
        if accion_es_arrancar(a) and args.arrancar:
            log(a)
        if accion_es_parar(a) and args.parar:
            log(a)
        if accion_es_giro(a) and args.giro:
            log(a)

if __name__ == "__main__":
    main()
