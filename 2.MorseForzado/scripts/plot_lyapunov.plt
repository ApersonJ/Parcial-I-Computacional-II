set terminal pngcairo size 1000,500 enhanced font 'Verdana,10'
set output 'results/figuras/lyapunov.png'
set title 'Mayor Exponente de Lyapunov vs Amplitud de Forzamiento'
set xlabel 'Amplitud (A)'
set ylabel '{/Symbol l}_{max}'
set grid
set xzeroaxis linetype 1 linecolor rgb 'black' linewidth 2
plot 'results/datos/lyapunov.dat' using 1:2 with linespoints pt 7 ps 0.5 lc rgb 'red' title 'Exponente {/Symbol l}'
