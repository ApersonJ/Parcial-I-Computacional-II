set terminal pngcairo size 800,600 enhanced font 'Verdana,10'
set output 'results/figuras/potencial_vs_posicion.png'
set title 'Oscilador de Morse: Energia Potencial vs Posicion'
set xlabel 'Posicion (x)'
set ylabel 'Energia Potencial (P)'
set grid
plot 'results/datos/trayectoria.dat' using 2:4 with lines title 'P(x)' linecolor rgb 'forest-green'
