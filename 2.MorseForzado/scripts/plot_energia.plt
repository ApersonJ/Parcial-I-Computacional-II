set terminal pngcairo size 800,900 enhanced font 'Verdana,10'
set output 'results/figuras/energia.png'
set multiplot layout 2,1 title 'Oscilador de Morse: Diagnostico de energia'
set grid
set xlabel 'Tiempo (t)'
set ylabel 'Energia total E(t)'
unset title
plot 'results/datos/trayectoria.dat' using 1:5 with lines title 'E(t)' linecolor rgb 'dark-orange'
set ylabel 'Error relativo (E(t)-E0)/E0'
plot 'results/datos/trayectoria.dat' using 1:6 with lines title '(E-E0)/E0' linecolor rgb 'black'
unset multiplot
