set terminal pngcairo size 1000,700 enhanced font 'Verdana,10'
set output 'results/figuras/diagrama_bifurcacion.png'
set title 'Diagrama de Bifurcación: Oscilador de Morse Forzado'
set xlabel 'Amplitud de forzamiento (A)'
set ylabel 'Posición x en t = nT (Poincaré)'
set grid
plot 'results/datos/bifurcacion.dat' using 1:2 with points pt 7 ps 0.2 lc rgb 'black' notitle
