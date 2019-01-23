set title "pid 1"

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
plot '2019-01-23-pid-1.dat' using ($1 * 100) linecolor "red"
replot '2019-01-23-pid-1.dat' using ($2 * 100)  linecolor "dark-plum"
#replot '2019-01-23-pid-1.dat' using 3 linecolor "magenta"
replot '2019-01-23-pid-1.dat' using 4 linecolor "gray"
replot '2019-01-23-pid-1.dat' using 5 linecolor "salmon"
replot '2019-01-23-pid-1.dat' using 6 linecolor "light-coral"
replot '2019-01-23-pid-1.dat' using 7 linecolor "yellow4"
replot '2019-01-23-pid-1.dat' using ($8/10) linecolor "cyan"
#replot '2019-01-23-pid-1.dat' using 9 linecolor "sea-green"


# Espera a que se pulse una tecla
pause mouse keypress
