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
plot '/tmp/data2.dat' using 2 linecolor "red" title "vleft"
replot '/tmp/data2.dat' using 3  linecolor "dark-plum" title "vright"
replot '/tmp/data2.dat' using 6 linecolor "magenta" title "objetivo"
replot '/tmp/data2.dat' using 7 linecolor "gray" title "v calculada"
replot '/tmp/data2.dat' using ($8/5000) linecolor "salmon" title "encoders"
#replot '/tmp/data2.dat' using 6 linecolor "light-coral"
#replot '/tmp/data2.dat' using 7 linecolor "yellow4"
#replot '/tmp/data2.dat' using ($8/10) linecolor "cyan"
#replot '/tmp/data2.dat' using 9 linecolor "sea-green"


# Espera a que se pulse una tecla
pause mouse keypress
