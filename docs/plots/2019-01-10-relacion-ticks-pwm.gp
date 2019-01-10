# 2019-01-09

set title "ticks y pwm en funcion de ticks deseados - periodo 4ms" font ",18"

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
plot '2019-01-10-relacion-ticks-pwm.dat' using 1:1 linecolor "red"
replot '2019-01-10-relacion-ticks-pwm.dat' using 1:2 linecolor "blue"
replot '2019-01-10-relacion-ticks-pwm.dat' using 1:($3*500) linecolor "magenta"
#replot '2019-01-10-relacion-ticks-pwm.dat' using 1:($4*200) linecolor "cyan"
#replot '2019-01-10-relacion-ticks-pwm.dat' using 1:5 linecolor "yellow"
#replot '2019-01-10-relacion-ticks-pwm.dat' using 1:6 linecolor "olive"
replot '2019-01-10-relacion-ticks-pwm.dat' using 1:(abs($5-$6)*1000 + 5000) linecolor "cyan" lw 8 title "desvio"
replot '2019-01-10-relacion-ticks-pwm.dat' using 1:($7 * 1000) linecolor "red" with points

# Espera a que se pulse una tecla
pause mouse keypress

# Autoactualizar
pause 1
reread
