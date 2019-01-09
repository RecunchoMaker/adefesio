set title "Ejemplo de uso de gnuplot"

#set style data line
set style data linespoints

# Define un rango para los ejes (por defecto, automático)
# set xrange [0:50]
# set yrange [0:50]

set xlabel "eje x"
set ylabel "eje y"

# muestra una rejilla
set grid

# obtiene los titulos de las columnas del fichero de datos
set key autotitle columnhead

# para mostrar nombres de colores: show palette colornames
plot 'datos.dat' using 1:2 linecolor "red"
replot 'datos.dat' using 1:3 linecolor "blue"
replot 'datos.dat' using 1:4 linecolor "magenta"
replot 'datos.dat' using 2:4 linecolor "magenta"

# Espera a que se pulse una tecla
pause mouse keypress
