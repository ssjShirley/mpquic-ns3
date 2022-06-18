
#!/bin/bash

for i in 80 81 82 83 84 85 86 87 88 89
do
    FOLD="r10size$i"
    DIR="temp-result/$FOLD/"
    ./waf --run "scratch/gnuplot-scheduler-rxbytes.cc --Dir=$FOLD"
    gnuplot scheduler-rx.plt
    rm "${DIR}scheduler-rx.plt"
    rm "${DIR}scheduler-rx.png"
    cp scheduler-rx.plt "${DIR}scheduler-rx.plt"
    cp scheduler-rx.png "${DIR}scheduler-rx.png"
    rm scheduler-rx.plt
    rm scheduler-rx.png
done