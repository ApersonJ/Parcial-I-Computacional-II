reset
serie = '../results/serie_presion.dat'

stats serie using 1:2 nooutput
N    = STATS_records
tmax = STATS_max_x
ymin = STATS_min_y
ymax = STATS_max_y
pad  = (ymax > ymin) ? 0.05*(ymax-ymin) : 1.0

set title 'Presion vs tiempo'
set xlabel 't (s)'
set ylabel 'Presion'
set xrange [0:tmax]
set yrange [ymin-pad:ymax+pad]
set grid
unset key

set terminal gif animate delay 5 loop 0 size 800,500
set output '../results/animations/presion.gif'
do for [i=0:N-1] {
    plot serie every ::0::i using 1:2 with lines lw 2 lc rgb '#1f77b4' notitle, \
         serie every ::i::i using 1:2 with points pt 7 ps 1.5 lc rgb 'black' notitle
}
set output

set terminal pngcairo size 1000,600
set output '../results/graphics/presion.png'
plot serie using 1:2 with lines lw 2 lc rgb '#1f77b4' notitle, \
     serie every ::0::0 using 1:2 with points pt 7 ps 1.5 lc rgb 'green' notitle, \
     serie every ::N-1::N-1 using 1:2 with points pt 7 ps 1.5 lc rgb 'red' notitle
set output
