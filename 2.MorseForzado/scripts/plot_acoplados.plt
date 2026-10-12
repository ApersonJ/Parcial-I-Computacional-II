set terminal pngcairo size 1000,800 enhanced font 'Verdana,10'
set grid
set output 'results/figuras/acoplados/sincronizacion_k_1.5.png'
set multiplot layout 2,1 title 'Sincronizacion ({/Symbol k} = 1.5)'
set xlabel 'Tiempo (t)'
set ylabel 'Posiciones'
plot 'results/datos/acoplados.dat' using 1:2 w l lw 1.5 lc rgb 'blue' t 'x_1', \
     '' using 1:5 w l lw 1.5 lc rgb 'red' dt 2 t 'x_2'
set title 'Error e(t) = |x_1 - x_2|'
set ylabel 'Error'
set logscale y
set format y '10^{%L}'
plot 'results/datos/acoplados.dat' using 1:8 w l lc rgb 'black' t 'e(t)'
unset multiplot
unset logscale y
set format y '%g'
set output 'results/figuras/acoplados/dinamica_k_1.5.png'
set multiplot layout 3,1 title 'Evolucion Temporal ({/Symbol k} = 1.5)'
unset title
set xlabel 'Tiempo (t)'
set ylabel 'Posicion x'
plot 'results/datos/acoplados.dat' using 1:2 w l lc rgb 'blue' t 'x_1', '' using 1:5 w l lc rgb 'red' dt 2 t 'x_2'
set ylabel 'Velocidad v'
plot 'results/datos/acoplados.dat' using 1:3 w l lc rgb 'blue' t 'v_1', '' using 1:6 w l lc rgb 'red' dt 2 t 'v_2'
set ylabel 'Potencial P'
plot 'results/datos/acoplados.dat' using 1:4 w l lc rgb 'blue' t 'P_1', '' using 1:7 w l lc rgb 'red' dt 2 t 'P_2'
unset multiplot
set terminal pngcairo size 800,800 enhanced font 'Verdana,10'
set output 'results/figuras/acoplados/fase_k_1.5.png'
set title 'Espacio de Fase ({/Symbol k} = 1.5)'
set xlabel 'Posicion (x)'
set ylabel 'Velocidad (v)'
plot 'results/datos/acoplados.dat' using 2:3 w l lc rgb 'blue' t 'Oscilador 1', \
     '' using 5:6 w l lc rgb 'red' dt 2 t 'Oscilador 2'
