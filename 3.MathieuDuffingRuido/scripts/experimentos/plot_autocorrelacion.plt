set terminal pngcairo size 900,600 enhanced font 'Verdana,10'
set output 'results/figuras/experimentos/autocorrelacion.png'
set title 'Autocorrelacion normalizada de x(t)'
set xlabel '{/Symbol t}'
set ylabel 'C({/Symbol t}) / C(0)'
set grid
plot 'results/datos/experimentos/autocorrelacion_h_bajo.dat' using 1:2 with lines title 'h = 0.1, {/Symbol s} = 0.1', \
     'results/datos/experimentos/autocorrelacion_h_alto.dat' using 1:2 with lines title 'h = 0.5, {/Symbol s} = 0.05'
