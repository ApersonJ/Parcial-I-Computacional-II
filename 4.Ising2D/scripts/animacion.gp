set terminal gif animate delay 5 loop 0 size 600,600
set output '../results/animations/animacion.gif'
set size square
set palette defined (-1 '#1f77b4', 1 '#d62728')
set cbrange [-1:1]
set cbtics ('-1' -1, '+1' 1)
set xrange [-0.5:99.5]
set yrange [99.5:-0.5]
unset key

do for [k=0:500] {
    set title sprintf('MCS = %d', k*10)
    plot '../results/datos/animacion.dat' index k matrix with image
}
set output
