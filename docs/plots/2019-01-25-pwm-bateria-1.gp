set title "pid - bat a 7.94"

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
plot '2019-01-25-pwm-bateria-1.dat' using ($1 * 10) linecolor "red" title "ultima velocidad * 10"
replot '2019-01-25-pwm-bateria-1.dat' using ($2 * 10)  linecolor "dark-plum" title "velocidad deseada * 10"
#replot '2019-01-25-pwm-bateria-1.dat' using 3 linecolor "magenta"
replot '2019-01-25-pwm-bateria-1.dat' using 4 linecolor "gray"
replot '2019-01-25-pwm-bateria-1.dat' using ($5*1) linecolor "salmon" title "kp * 1"
replot '2019-01-25-pwm-bateria-1.dat' using ($6*1) linecolor "light-coral" title "kd * 1"
replot '2019-01-25-pwm-bateria-1.dat' using ($7*1) linecolor "yellow4" title "ki * 1"
replot '2019-01-25-pwm-bateria-1.dat' using ($8*1) linecolor "cyan" title "potencia * 1"
#replot '2019-01-24-mas-pid-3.dat' using 9 linecolor "sea-green"


# Espera a que se pulse una tecla
pause mouse keypress
