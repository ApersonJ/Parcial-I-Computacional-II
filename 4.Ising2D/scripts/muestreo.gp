set terminal pngcairo size 900,600 enhanced font 'Arial,12'
set grid
set xlabel 'MCS'

set output '../results/muestreo/E_vs_MCS.png'
set title 'Muestreo: Energia vs MCS'
set ylabel 'E'
plot '../results/datos/muestreo.dat' using 1:2 with lines lw 1.5 lc rgb '#d62728' notitle

set output '../results/muestreo/M_vs_MCS.png'
set title 'Muestreo: Magnetizacion vs MCS'
set ylabel 'M'
plot '../results/datos/muestreo.dat' using 1:3 with lines lw 1.5 lc rgb '#1f77b4' notitle

set output
