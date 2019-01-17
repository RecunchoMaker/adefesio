set title "KA"

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
plot '2019-01-17-nueva-kp.dat' using 1 linecolor "red"
replot '2019-01-17-nueva-kp.dat' using 2 linecolor "blue"
replot '2019-01-17-nueva-kp.dat' using ($3/1000) linecolor "brown"
replot '2019-01-17-nueva-kp.dat' using 7 linecolor "green"

# Espera a que se pulse una tecla
pause mouse keypress
