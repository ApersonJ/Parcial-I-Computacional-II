set terminal pngcairo size 900,600 enhanced font 'Verdana,10'
set output 'results/figuras/histograma.png'
set title 'Distribucion de x al final de la simulacion'
set xlabel 'Posicion (x)'
set ylabel 'P(x, t_{max})'
set grid
stats 'results/datos/x_final.dat' using 1 nooutput
bw = (STATS_max - STATS_min) / 40.0
if (bw == 0) bw = 1.0
bin(x) = bw * floor(x / bw) + bw / 2.0
set boxwidth bw
set style fill solid 0.5
plot 'results/datos/x_final.dat' using (bin($1)):(1.0 / (STATS_records * bw)) smooth freq with boxes lc rgb 'forest-green' notitle
