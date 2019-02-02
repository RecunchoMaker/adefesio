set title "pid"

set style data line
#set style data linespoints

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
# plot '/tmp/data.dat' using 1 linecolor "red"
plot '/tmp/data.dat' using 2 linecolor "blue"
#replot '/tmp/data.dat' using 3 linecolor "brown"
replot '/tmp/data.dat' using 4 linecolor "green"
#replot '/tmp/data.dat' using 5 linecolor "olive"

# Espera a que se pulse una tecla
pause mouse keypress
