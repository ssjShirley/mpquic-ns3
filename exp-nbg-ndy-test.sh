
#!/bin/bash

FOLD="test-nbg-ndy-$2-$3-$1"
DIR="temp-result/$FOLD/"
RATE0="$2Mbps"
RATE1="50Mbps"
DELAY0="$3ms"
DELAY1="20ms"
LOSS="0.000000"
BLambda="20"
BVar="0.2"
MRate="26214400"
# MRate="52428800"

# mkdir ${DIR}
# for i in 0 1 2 3 4
for i in 4
do
    LOG="6"
    LOG1="scheduler${LOG}-queue.txt"
    LOG2="scheduler${LOG}-flowsum.txt"
    ./waf --run "scratch/mpquic-scheduler-nbg-ndy.cc --Size=$1 --BVar=$BVar --BLambda=$BLambda --MabRate=$MRate --SchedulerType=$i --Rate0=${RATE0} --Rate1=${RATE1} --Delay0=${DELAY0} --Delay1=${DELAY1} --LossRate=$LOSS" >$LOG1 2>$LOG2

    FILE="scheduler$i-reward.txt"
    FILE1="scheduler${LOG}-reward.txt"
    cp $FILE "${DIR}${FILE1}"
    rm $FILE

    FILE="scheduler$i-cwnd-change-0.txt"
    FILE1="scheduler${LOG}-cwnd-change-0.txt"
    cp $FILE "${DIR}${FILE1}"
    rm $FILE

    FILE="scheduler$i-cwnd-change-1.txt"
    FILE1="scheduler${LOG}-cwnd-change-1.txt"
    cp $FILE "${DIR}${FILE1}"
    rm $FILE

    FILE="scheduler$i-rtt-change-0.txt"
    FILE1="scheduler${LOG}-rtt-change-0.txt"
    cp $FILE "${DIR}${FILE1}"
    rm $FILE

    FILE="scheduler$i-rtt-change-1.txt"
    FILE1="scheduler${LOG}-rtt-change-1.txt"
    cp $FILE "${DIR}${FILE1}"
    rm $FILE

    FILE="scheduler$i-rx.txt"
    FILE1="scheduler${LOG}-rx.txt"
    cp $FILE "${DIR}${FILE1}"
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