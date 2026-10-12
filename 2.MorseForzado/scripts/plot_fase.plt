set terminal pngcairo size 800,600 enhanced font 'Verdana,10'
set output 'results/figuras/espacio_fase.png'
set title 'Oscilador de Morse: Espacio de Fase'
set xlabel 'Posicion (x)'
set ylabel 'Velocidad (v)'
set grid
plot 'results/datos/trayectoria.dat' using 2:3 with lines title 'v(x)' linecolor rgb 'purple'
