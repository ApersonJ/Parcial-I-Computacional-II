set terminal pngcairo size 800,800 enhanced font 'Verdana,10'
set output 'results/figuras/experimentos/estroboscopico.png'
set title 'Mapa estroboscopico (x, v) en t = nT, h = 0.5'
set xlabel 'x(nT)'
set ylabel 'v(nT)'
set grid
plot 'results/datos/experimentos/estroboscopico_s02.dat' using 1:2 with points pt 7 ps 0.3 lc rgb 'red' title '{/Symbol s} = 0.2', \
     'results/datos/experimentos/estroboscopico_s005.dat' using 1:2 with points pt 7 ps 0.3 lc rgb 'blue' title '{/Symbol s} = 0.05', \
     'results/datos/experimentos/estroboscopico_s0.dat' using 1:2 with points pt 7 ps 1.5 lc rgb 'black' title '{/Symbol s} = 0 (determinista)'
