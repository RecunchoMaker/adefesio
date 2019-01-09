# 2019-01-09

set title "ticks y pwm en funcion de ticks deseados" font ",18"

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
plot '2019-01-09-relacion-ticks-pwm.dat' using 1:1 linecolor "red"
replot '2019-01-09-relacion-ticks-pwm.dat' using 1:2 linecolor "blue"
replot '2019-01-09-relacion-ticks-pwm.dat' using 1:($3*200) linecolor "magenta"
replot '2019-01-09-relacion-ticks-pwm.dat' using 1:($4*200) linecolor "cyan"

# Espera a que se pulse una tecla
pause mouse keypress
