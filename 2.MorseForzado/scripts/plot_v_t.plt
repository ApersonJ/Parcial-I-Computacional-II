set terminal pngcairo size 800,600 enhanced font 'Verdana,10'
set output 'results/figuras/velocidad_vs_tiempo.png'
set title 'Oscilador de Morse: Velocidad vs Tiempo'
set xlabel 'Tiempo (t)'
set ylabel 'Velocidad (v)'
set grid
plot 'results/datos/trayectoria.dat' using 1:3 with lines title 'v(t)' linecolor rgb 'red'
