set terminal pngcairo size 700,700 enhanced font 'Arial,12'
set size square
set palette defined (-1 '#1f77b4', 1 '#d62728')
set cbrange [-1:1]
set cbtics ('-1' -1, '+1' 1)
set xrange [-0.5:49.5]
set yrange [49.5:-0.5]
unset key
set xlabel 'j'
set ylabel 'i'

set output '../results/red/red_inicial.png'
set title 'Red inicial'
plot '../results/datos/red_inicial.dat' matrix with image

set output '../results/red/red_termalizada.png'
set title 'Red despues de la termalizacion'
plot '../results/datos/red_termalizada.dat' matrix with image

set output '../results/red/red_muestreo.png'
set title 'Red despues del muestreo'
plot '../results/datos/red_muestreo.dat' matrix with image

set output
