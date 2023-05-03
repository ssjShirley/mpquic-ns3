
#!/bin/bash

FOLD="cdynamic0-$1"
DIR="results-globecom/$FOLD/"
RATE0a="5.0"
RATE0b="5.05"
RATE1a="10.0"
RATE1b="10.1"
DELAY0a="10.0"
DELAY0b="10.1"
DELAY1a="50.0"
DELAY1b="50.5"
LOSS="0.000001"
BLambda="200"
BVar="0"
Size=5242800
SEED=$1

mkdir ${DIR}
for i in {0..5}
do
    LOG="$i"
    LOG1="scheduler${LOG}-queue.txt"
    LOG2="scheduler${LOG}-flowsum.txt"
    ./waf --run "scratch/msab-example.cc --Seed=$SEED --Size=$Size --BVar=$BVar --BLambda=$BLambda --SchedulerType=${LOG} --Rate0a=${RATE0a} --Rate1a=${RATE1a} --Delay0a=${DELAY0a} --Delay1a=${DELAY1a} --Rate0b=${RATE0b} --Rate1b=${RATE1b} --Delay0b=${DELAY0b} --Delay1b=${DELAY1b} --LossRate=$LOSS" >$LOG1 2>$LOG2

    FILE="scheduler${LOG}-rx.txt"
    cp $FILE "${DIR}${FILE}"
    rm $FILE

    cp $LOG1 "${DIR}${LOG1}"
    rm $LOG1

    cp $LOG2 "${DIR}${LOG2}"
    rm $LOG2

done
