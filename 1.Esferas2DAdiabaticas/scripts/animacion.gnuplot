set terminal gif animate delay 5 optimize size 800,600
set output '../results/animacion.gif'

set xrange [0:10]
set yrange [0:10]
set size ratio -1
unset key
set xlabel 'x'
set ylabel 'y'
set grid

do for [i=0:60] {
    stats '../results/frames.dat' index i using 4 nooutput
    pared = STATS_min

    plot '../results/frames.dat' index i using 2:3 with points pt 7 ps 1 notitle, \
         '-' using 1:2 with lines lw 3 notitle, \
         '-' using 1:2 with lines lw 3 notitle, \
         '-' using 1:2 with lines lw 3 notitle, \
         '-' using 1:2 with lines lw 3 notitle
    pared 0
    pared 10
    e
    10 0
    10 10
    e
    pared 0
    10 0
    e
    pared 10
    10 10
    e
}
set output
