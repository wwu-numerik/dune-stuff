
set title ""
set xlabel "time"
set ylabel "x"
set zlabel "u(x)"
set view 59, 77, 1, 1
splot "solu_000000_current__velocity.gnu" using 3:1:2 with lines