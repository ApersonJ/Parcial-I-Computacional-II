set terminal pngcairo size 900,600 enhanced font 'Verdana,10'
set output 'results/figuras/experimentos/espectro.png'
set title 'Espectro de potencia de x(t)'
set xlabel '{/Symbol w}'
set ylabel 'S({/Symbol w})'
set logscale y
set grid
plot 'results/datos/experimentos/espectro_h_bajo.dat' using 1:2 with lines title 'h = 0.1, {/Symbol s} = 0.1', \
     'results/datos/experimentos/espectro_h_alto.dat' using 1:2 with lines title 'h = 0.5, {/Symbol s} = 0.05'
