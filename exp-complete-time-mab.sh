
#!/bin/bash

FOLD="com0-$2-$3-$4-$5"
LOSS="0.00000"
RVar="0.0"
DIR="results-ipccc/$FOLD/"
RATE0="$2Mbps"
RATE1="$3Mbps"
DELAY0="$4ms"
DELAY1="$5ms"
BLambda="100"
BVar="1"
MRate="10485760"

for i in 4
# for i in 0 2
do
    LOG="$i"
    LOG1="scheduler${LOG}-queue.txt"
    LOG2="scheduler${LOG}-flowsum.txt"
    ./waf --run "scratch/mpquic-scheduler-nbg-ndy.cc --Size=$1 --BVar=$BVar --BLambda=$BLambda --MabRate=$MRate --SchedulerType=$i{LOG} --Rate0=${RATE0} --Rate1=${RATE1} --Delay0=${DELAY0} --Delay1=${DELAY1} --LossRate=$LOSS" >$LOG1 2>$LOG2
    # ./waf --run "scratch/mpquic-scheduler-nbg-dy.cc --Size=$1 --BVar=$BVar --RVar=$RVar --BLambda=$BLambda --MabRate=$MRate --SchedulerType=$i{LOG} --Rate0=${RATE0} --Rate1=${RATE1} --Delay0=${DELAY0} --Delay1=${DELAY1} --LossRate=$LOSS" >$LOG1 2>$LOG2

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

FOLD="com1-$2-$3-$4-$5"
LOSS="0.000001"
RVar="0.1"
DIR="results-ipccc/$FOLD/"


for i in 4
# for i in 0 2
do
    LOG="$i"
    LOG1="scheduler${LOG}-queue.txt"
    LOG2="scheduler${LOG}-flowsum.txt"
    # ./waf --run "scratch/mpquic-scheduler-nbg-ndy.cc --Size=$1 --BVar=$BVar --BLambda=$BLambda --MabRate=$MRate --SchedulerType=$i{LOG} --Rate0=${RATE0} --Rate1=${RATE1} --Delay0=${DELAY0} --Delay1=${DELAY1} --LossRate=$LOSS" >$LOG1 2>$LOG2
    ./waf --run "scratch/mpquic-scheduler-nbg-dy.cc --Size=$1 --BVar=$BVar --RVar=$RVar --BLambda=$BLambda --MabRate=$MRate --SchedulerType=$i{LOG} --Rate0=${RATE0} --Rate1=${RATE1} --Delay0=${DELAY0} --Delay1=${DELAY1} --LossRate=$LOSS" >$LOG1 2>$LOG2

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


FOLD="com2-$2-$3-$4-$5"
LOSS="0.000005"
RVar="0.2"
DIR="results-ipccc/$FOLD/"



for i in 4
# for i in 0 2
do
    LOG="$i"
    LOG1="scheduler${LOG}-queue.txt"
    LOG2="scheduler${LOG}-flowsum.txt"
    # ./waf --run "scratch/mpquic-scheduler-nbg-ndy.cc --Size=$1 --BVar=$BVar --BLambda=$BLambda --MabRate=$MRate --SchedulerType=$i{LOG} --Rate0=${RATE0} --Rate1=${RATE1} --Delay0=${DELAY0} --Delay1=${DELAY1} --LossRate=$LOSS" >$LOG1 2>$LOG2
    ./waf --run "scratch/mpquic-scheduler-nbg-dy.cc --Size=$1 --BVar=$BVar --RVar=$RVar --BLambda=$BLambda --MabRate=$MRate --SchedulerType=$i{LOG} --Rate0=${RATE0} --Rate1=${RATE1} --Delay0=${DELAY0} --Delay1=${DELAY1} --LossRate=$LOSS" >$LOG1 2>$LOG2

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