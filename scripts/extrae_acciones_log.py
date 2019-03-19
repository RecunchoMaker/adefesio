#!/usr/bin/env python
import sys
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

    # Sin parametros, se extrae todo
    args.todos = (not args.recto and not args.arrancar and \
                  not args.parar and not args.giro)
    return args


""" Devuelve True si una linea tiene el numero de elementos correctos """
def es_linea_correcta(linea):

    linea = linea.rstrip('\r').rstrip('\n')

    # Es un log?
    linea = linea.split(' ')

    if len(linea)<>2:
        return False

    if linea[0] <> '#1':
        return False

    # Tiene el numero de valores correcto?
    linea = linea[1].split('\t')
    if len(linea) <> 7:
        return False

    # Son todos los valores enteros?
    try:
        linea = [int(linea[0]),
                 int(linea[1]),
                 int(linea[2]),
                 int(linea[3]),
                 int(linea[4]),
                 int(linea[5]),
                 float(linea[6])]
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
            float(l[6])]


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
        print "%d\t%d\t%d\t%d\t%d\t%d\t%0.2f" % (a[0],a[1],a[2],a[3],a[4],a[5],a[6])


def main():
    args = parse_options()

    lineas = extraer_lineas_correctas(args.FICHERO)
    if not args.quiet:
        sys.stderr.write("%d lineas correctas en '%s'\n" %
                         (len(lineas), args.FICHERO))

    if lineas:
        acciones = extraer_acciones(lineas)
        cuenta = 0
        for a in acciones:
            cuenta = cuenta + 1
            if (args.todos):
                log(a)
            else:
                if accion_es_recto(a) and args.recto:
                    log(a)
                if accion_es_arrancar(a) and args.arrancar:
                    log(a)
                if accion_es_parar(a) and args.parar:
                    log(a)
                if accion_es_giro(a) and args.giro:
                    log(a)
        if not args.quiet:
            sys.stderr.write("%d acciones en '%s'\n" %
                             (len(acciones),args.FICHERO))


if __name__ == "__main__":
    main()
