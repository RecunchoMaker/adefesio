set title "velocidad-pwm"

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
plot '2019-01-16-velocidad-pwm.dat' using ($1*1000) linecolor "red"
replot '2019-01-16-velocidad-pwm.dat' using ($2*1000) linecolor "blue"
replot '2019-01-16-velocidad-pwm.dat' using 3 linecolor "green"
replot '2019-01-16-velocidad-pwm.dat' using 4 linecolor "olive"
#replot '2019-01-16-velocidad-pwm.dat' using ($5/10) linecolor "grey"
#replot '2019-01-16-velocidad-pwm.dat' using ($6/10) linecolor "orange"
replot '2019-01-16-velocidad-pwm.dat' using ($3/$1) linecolor "olive" title "pwm / velocidad"

# Espera a que se pulse una tecla
pause mouse keypress
