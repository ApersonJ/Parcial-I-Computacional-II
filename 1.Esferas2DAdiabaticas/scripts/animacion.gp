# Animacion generada automaticamente
reset
L = 50
h = 50
archivo = '../results/frames.dat'

stats archivo nooutput
N = STATS_blocks

set terminal gif animate delay 5 loop 0 size 900,900*h/L+80 
set output '../results/animations/animacion.gif'

set size ratio h/L
set xrange [-0.05*L:1.05*L]
set yrange [-0.05*h:1.10*h]
set xlabel 'x (m)'
set ylabel 'y (m)'
unset key
set style fill solid 0.6 border lc rgb 'black'

set object 1 rectangle from 0,0 to L,h fs empty border lc rgb 'black' lw 2 front

do for [i=0:N-1] {
    plot archivo index i using 2:3:4 with circles lc rgb '#1f77b4' notitle, \
         archivo index i every ::0::0 using 5:(0):(0):(h) with vectors nohead lw 3 lc rgb 'red' notitle, \
         archivo index i every ::0::0 using (0.5*L):(1.03*h):(sprintf('t = %.2f s', $1)) with labels center notitle
}
set output
