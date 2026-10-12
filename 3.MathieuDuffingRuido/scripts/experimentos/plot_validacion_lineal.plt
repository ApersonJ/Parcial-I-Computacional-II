set terminal pngcairo size 900,600 enhanced font 'Verdana,10'
set output 'results/figuras/experimentos/validacion_lineal.png'
set title 'Limite lineal: Var[x] estacionaria vs paso dt'
set xlabel 'Paso dt'
set ylabel 'Var[x]'
set grid
set key left top
plot 'results/datos/experimentos/validacion_lineal.dat' using 1:2 with linespoints title 'Euler-Maruyama', \
     '' using 1:3 with linespoints title 'Heun', \
     0.25 with lines lc rgb 'black' dt 2 title 'teoria'
