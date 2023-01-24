
#!/bin/bash

FOLD="comeon0-$3-$1"
LOSS="0.00000"
RVar="0.0"
DIR="temp-result/$FOLD/"
RATE0="$3Mbps"
RATE1="10Mbps"
DELAY0="$2ms"
DELAY1="10ms"
BLambda="100"
BVar="1"
MRate="26214400"


mkdir ${DIR}
for i in 0 1 2 3
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

FOLD="comeon1-$3-$1"
LOSS="0.000001"
RVar="0.1"
DIR="temp-result/$FOLD/"
RATE0="$3Mbps"
RATE1="10Mbps"
DELAY0="$2ms"
DELAY1="10ms"
BLambda="100"
BVar="1"
MRate="26214400"


mkdir ${DIR}
for i in 0 1 2 3
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


FOLD="comeon2-$3-$1"
LOSS="0.000005"
RVar="0.2"
DIR="temp-result/$FOLD/"
RATE0="$3Mbps"
RATE1="10Mbps"
DELAY0="$2ms"
DELAY1="10ms"
BLambda="100"
BVar="1"
MRate="26214400"


mkdir ${DIR}
for i in 0 1 2 3
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