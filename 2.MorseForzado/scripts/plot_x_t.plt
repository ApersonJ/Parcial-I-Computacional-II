set terminal pngcairo size 800,600 enhanced font 'Verdana,10'
set output 'results/figuras/posicion_vs_tiempo.png'
set title 'Oscilador de Morse: Posicion vs Tiempo'
set xlabel 'Tiempo (t)'
set ylabel 'Posicion (x)'
set grid
plot 'results/datos/trayectoria.dat' using 1:2 with lines title 'x(t)' linecolor rgb 'blue'
