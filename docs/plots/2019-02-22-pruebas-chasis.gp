set title "despues"

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
plot '/tmp/data.dat' using 3 linecolor "red" title "vleft"
replot '/tmp/data.dat' using 4  linecolor "dark-plum" title "vright"
replot '/tmp/data.dat' using 5 linecolor "magenta" title "objetivo"
replot '/tmp/data.dat' using ($6/20000) linecolor "gray"
#replot '/tmp/data.dat' using 7 linecolor "blue" title "vreal" with points

#replot 'despues.dat' using 5 linecolor "salmon"
#replot 'despues.dat' using 6 linecolor "light-coral"
#replot 'despues.dat' using 7 linecolor "yellow4"
#replot 'despues.dat' using ($8/10) linecolor "cyan"
#replot 'despues.dat' using 9 linecolor "sea-green"


# Espera a que se pulse una tecla
pause mouse keypress
