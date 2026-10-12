set terminal pngcairo size 900,600 enhanced font 'Verdana,10'
set output 'results/figuras/experimentos/convergencia_fuerte.png'
set title 'Convergencia fuerte (mismo ruido): error en x(T)'
set xlabel 'Paso dt'
set ylabel 'E|x(T) - x_{ref}(T)|'
set logscale xy
set grid
set key left top
plot 'results/datos/experimentos/convergencia_fuerte.dat' using 1:2 with linespoints title 'Euler-Maruyama', \
     '' using 1:3 with linespoints title 'Heun'
