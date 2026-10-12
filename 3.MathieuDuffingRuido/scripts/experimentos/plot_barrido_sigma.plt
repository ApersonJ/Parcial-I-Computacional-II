set terminal pngcairo size 900,600 enhanced font 'Verdana,10'
set output 'results/figuras/experimentos/barrido_sigma.png'
set title 'Respuesta cuadratica media vs intensidad del ruido'
set xlabel '{/Symbol s}'
set ylabel '<x^2>'
set grid
set key left top
plot 'results/datos/experimentos/barrido_sigma.dat' using 1:2 with linespoints title 'h = 0.1 (bajo el umbral)', \
     '' using 1:3 with linespoints title 'h = 0.5 (sobre el umbral)'
