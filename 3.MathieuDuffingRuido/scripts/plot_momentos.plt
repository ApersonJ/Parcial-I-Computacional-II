set terminal pngcairo size 900,800 enhanced font 'Verdana,10'
set output 'results/figuras/momentos.png'
set multiplot layout 2,1 title 'Momentos del ensamble'
set grid
set xlabel 'Tiempo (t)'
set ylabel '<x>'
plot 'results/datos/momentos.dat' using 1:2 with lines lc rgb 'blue' notitle
set ylabel 'Var[x]'
plot 'results/datos/momentos.dat' using 1:3 with lines lc rgb 'red' notitle
unset multiplot
