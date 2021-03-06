set title "log casilla pasos led"

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

set angles degrees

# para mostrar nombres de colores: show palette colornames
plot '/tmp/datos-1.dat' using ($1*100) linecolor "pink" title "casilla" with lines
replot '/tmp/datos-1.dat' using 2 linecolor "pink" title "pasos" with points 
replot '/tmp/datos-1.dat' using ($3 * 1000) linecolor "magenta" title "IZQ (mm)" with points
replot '/tmp/datos-1.dat' using ($4 * 1000) linecolor "magenta" title "FIZQ (mm)" with lines
replot '/tmp/datos-1.dat' using ($5 * 1000) linecolor "red" title "FDER (mm)" with lines
replot '/tmp/datos-1.dat' using ($6 * 1000) linecolor "red" title "DER (mm)" with points
replot '/tmp/datos-1.dat' using (10*atan($7)) linecolor "slategrey" title "desvio" with points

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
