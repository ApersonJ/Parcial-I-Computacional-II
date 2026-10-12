set terminal pngcairo size 900,600 enhanced font 'Verdana,10'
set output 'results/figuras/trayectorias.png'
set title 'Mathieu-Duffing con ruido: realizaciones y media'
set xlabel 'Tiempo (t)'
set ylabel 'Posicion (x)'
set grid
plot for [c=2:6] 'results/datos/trayectorias.dat' using 1:c with lines lw 1 title sprintf('realizacion %d', c-1), \
     'results/datos/momentos.dat' using 1:2 with lines lw 3 lc rgb 'black' title '<x>(t)'
