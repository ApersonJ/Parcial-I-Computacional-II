set terminal pngcairo size 900,600 enhanced font 'Verdana,10'
set output 'results/figuras/experimentos/convergencia.png'
set title 'Convergencia: error en x(t_{max}) vs paso dt'
set xlabel 'Paso dt'
set ylabel '|x(t_{max}) - x_{ref}|'
set logscale xy
set grid
set key left top
plot 'results/datos/experimentos/convergencia.dat' using 1:2 with linespoints title 'Euler', \
     '' using 1:3 with linespoints title 'Euler-Cromer', \
     '' using 1:4 with linespoints title 'RK2', \
     '' using 1:5 with linespoints title 'RK3', \
     '' using 1:6 with linespoints title 'RK4' linecolor rgb 'black'
