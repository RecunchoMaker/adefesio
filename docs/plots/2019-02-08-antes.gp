set title "frenado"

#set style data line
set style data linespoints

# Define un rango para los ejes (por defecto, automático)
# set xrange [0:50]
# set yrange [0:50]

set xlabel ""
set ylabel ""

# muestra una rejilla
set grid

# obtiene los titulos de las columnas del fichero de datos
set key autotitle columnhead

# para mostrar nombres de colores: show palette colornames
plot '2019-02-08-antes.dat' using 3 linecolor "red" title "vleft"
replot '2019-02-08-antes.dat' using 4  linecolor "dark-plum" title "vright"
replot '2019-02-08-antes.dat' using 5 linecolor "magenta" title "objetivo"
replot '2019-02-08-antes.dat' using ($6/20000) linecolor "gray"
#replot 'antes.dat' using 5 linecolor "salmon"
#replot 'antes.dat' using 6 linecolor "light-coral"
#replot 'antes.dat' using 7 linecolor "yellow4"
#replot 'antes.dat' using ($8/10) linecolor "cyan"
#replot 'antes.dat' using 9 linecolor "sea-green"


# Espera a que se pulse una tecla
pause mouse keypress
