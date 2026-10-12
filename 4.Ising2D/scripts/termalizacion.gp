set terminal pngcairo size 900,600 enhanced font 'Arial,12'
set grid
set xlabel 'MCS'

set output '../results/termalizacion/E_vs_MCS.png'
set title 'Termalizacion: Energia vs MCS'
set ylabel 'E'
plot '../results/datos/termalizacion.dat' using 1:2 with lines lw 1.5 lc rgb '#d62728' notitle

set output '../results/termalizacion/M_vs_MCS.png'
set title 'Termalizacion: Magnetizacion vs MCS'
set ylabel 'M'
plot '../results/datos/termalizacion.dat' using 1:3 with lines lw 1.5 lc rgb '#1f77b4' notitle

set output
