set terminal pngcairo size 900,600 enhanced font 'Arial,12'
set grid
set output '../results/termalizacion/Inv_vs_MCS.png'
set title 'Termalizacion: spines invertidos vs MCS'
set xlabel 'MCS'
set ylabel 'Numero de spines con -1'
plot '../results/datos/termalizacion.dat' using 1:4 with lines lw 1.5 lc rgb '#2ca02c' notitle
set output
