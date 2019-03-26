set title "calibracion distancia"

#set style data line

set style line 10 lw 5 pt 3 linetype 2 linecolor rgb "blue"

# Define un rango para los ejes (por defecto, automático)
# set xrange [0:50]
# set yrange [0:50]

set xlabel ""
set ylabel ""

# muestra una rejilla
set grid

# obtiene los titulos de las columnas del fichero de datos
set key autotitle columnhead

set key left top

# para mostrar nombres de colores: show palette colornames

# Curvas desplazadas intantando que coincidan
plot 'sensores-todos.dat' using ($1-11):2 linecolor "red" title "izquierda" with points
replot 'sensores-todos.dat' using 1:3 linecolor "blue" title "f-izquierda" with points 
replot 'sensores-todos.dat' using ($1-5):4 linecolor "magenta" title "f-derecha" with points
replot 'sensores-todos.dat' using ($1-3):5 linecolor "red" title "derecha" with points

# Funcion de prueba que coincide con la curva de los sensores
#a = 0.9
#f(x) = 1.037**x + 25
#replot f(x) title '1.037**x + 25' with lines ls 10

# Curvas originales
#plot 'sensores-todos.dat' using 1:2 linecolor "red" title "izquierda" with points
#replot 'sensores-todos.dat' using 1:3 linecolor "blue" title "f-izquierda" with points 
#replot 'sensores-todos.dat' using 1:4 linecolor "magenta" title "f-derecha" with points
#replot 'sensores-todos.dat' using 1:5 linecolor "red" title "derecha" with points

# Espera a que se pulse una tecla
pause mouse keypress

# chuleta colores

#  yellow4            #808000 = 128 128   0
#  sienna1            #ff8040 = 255 128  64
#  tan1               #ffa040 = 255 160  64
#  sandybrown         #ffa060 = 255 160  96
#  light-salmon       #ffa070 = 255 160 112
#  pink               #ffc0c0 = 255 192 192
#  khaki1             #ffff80 = 255 255 128
#  lemonchiffon       #ffffc0 = 255 255 192
#  bisque             #cdb79e = 205 183 158
#  honeydew           #f0fff0 = 240 255 240
#  slategrey          #a0b6cd = 160 182 205
#  seagreen           #c1ffc1 = 193 255 193
#  antiquewhite       #cdc0b0 = 205 192 176
#  chartreuse         #7cff40 = 124 255  64
#  greenyellow        #a0ff20 = 160 255  32
#  gray               #bebebe = 190 190 190
#  light-gray         #d3d3d3 = 211 211 211
#  light-grey         #d3d3d3 = 211 211 211
#  dark-gray          #a0a0a0 = 160 160 160
#  slategray          #a0b6cd = 160 182 205
