set terminal pngcairo size 900,600 enhanced font 'Verdana,10'
set output 'results/figuras/experimentos/error_vs_tiempo.png'
set title 'Error respecto a la referencia RK4 (dt = 0.0005)'
set xlabel 'Tiempo (t)'
set ylabel '|x - x_{ref}|'
set logscale y
set grid
plot 'results/datos/experimentos/error_vs_t.dat' using 1:2 with lines title 'Euler', \
     '' using 1:3 with lines title 'Euler-Cromer', \
     '' using 1:4 with lines title 'RK2', \
     '' using 1:5 with lines title 'RK3', \
     '' using 1:6 with lines title 'RK4' linecolor rgb 'black'
