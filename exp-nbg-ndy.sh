
#!/bin/bash

FOLD="test1-nbg-ndy-$2-$3-$1"
DIR="temp-result/$FOLD/"
RATE0="10Mbps"
RATE1="$2Mbps"
DELAY0="100ms"
DELAY1="$3ms"
LOSS="0.000000"
BLambda="20"
BVar="2"
MRate="26214400"

mkdir ${DIR}
for i in 0 1 2 3 4
# for i in 2
do
    LOG="$i"
    LOG1="scheduler${LOG}-queue.txt"
    LOG2="scheduler${LOG}-flowsum.txt"
    ./waf --run "scratch/mpquic-scheduler-nbg-ndy.cc --Size=$1 --BVar=$BVar --BLambda=$BLambda --MabRate=$MRate --SchedulerType=$i{LOG} --Rate0=${RATE0} --Rate1=${RATE1} --Delay0=${DELAY0} --Delay1=${DELAY1} --LossRate=$LOSS" >$LOG1 2>$LOG2

    FILE="scheduler${LOG}-reward.txt"
    cp $FILE "${DIR}${FILE}"
    rm $FILE

    FILE="scheduler${LOG}-cwnd-change-0.txt"
    cp $FILE "${DIR}${FILE}"
    rm $FILE

    FILE="scheduler${LOG}-cwnd-change-1.txt"
    cp $FILE "${DIR}${FILE}"
    rm $FILE

    FILE="scheduler${LOG}-rtt-change-0.txt"
    cp $FILE "${DIR}${FILE}"
    rm $FILE

    FILE="scheduler${LOG}-rtt-change-1.txt"
    cp $FILE "${DIR}${FILE}"
    rm $FILE

    FILE="scheduler${LOG}-rx.txt"
    cp $FILE "${DIR}${FILE}"
    rm $FILE

    cp $LOG1 "${DIR}${LOG1}"
    rm $LOG1

    cp $LOG2 "${DIR}${LOG2}"
    rm $LOG2

done

# ./waf --run "scratch/gnuplot-scheduler-rxbytes.cc --Dir=$FOLD"
# gnuplot scheduler-rx.plt
# cp scheduler-rx.plt "${DIR}scheduler-rx.plt"
# cp scheduler-rx.png "${DIR}scheduler-rx.png"
# rm scheduler-rx.plt
# rm scheduler-rx.png