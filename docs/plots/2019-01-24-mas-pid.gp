set title "pid 1"

#set style data line
set style data linespoints

# Define un rango para los ejes (por defecto, automático)
# set xrange [0:50]
# set yrange [-1:30]

set xlabel ""
set ylabel ""

# muestra una rejilla
set grid

# obtiene los titulos de las columnas del fichero de datos
set key autotitle columnhead

# para mostrar nombres de colores: show palette colornames
plot '2019-01-24-mas-pid-3.dat' using ($1 * 100) linecolor "red"
replot '2019-01-24-mas-pid-3.dat' using ($2 * 100)  linecolor "dark-plum"
#replot '2019-01-24-mas-pid-3.dat' using 3 linecolor "magenta"
replot '2019-01-24-mas-pid-3.dat' using 4 linecolor "gray"
replot '2019-01-24-mas-pid-3.dat' using 5 linecolor "salmon" title "kp / 10"
replot '2019-01-24-mas-pid-3.dat' using ($6/10) linecolor "light-coral" title "kd / 10"
replot '2019-01-24-mas-pid-3.dat' using ($7/10) linecolor "yellow4" title "ki / 10"
replot '2019-01-24-mas-pid-3.dat' using ($8/10) linecolor "cyan" title "pwm / 10"
#replot '2019-01-24-mas-pid-3.dat' using 9 linecolor "sea-green"


# Espera a que se pulse una tecla
pause mouse keypress
