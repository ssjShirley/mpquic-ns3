
#!/bin/bash

FOLD="four-c-$1"
DIR="results-globecom/$FOLD/"
RATE0="5.0"
RATE1="6.0"
DELAY0="10.0"
DELAY1="12.0"
LOSS="0.000005"
BLambda="200"
BVar="0"
Size=5242800
SEED=$1

mkdir ${DIR}
for i in 0 1 4 5
do
    LOG="$i"
    LOG1="scheduler${LOG}-queue.txt"
    LOG2="scheduler${LOG}-flowsum.txt"
    ./waf --run "scratch/msab-four-path.cc --Seed=$SEED --Size=$Size --BVar=$BVar --BLambda=$BLambda --SchedulerType=${LOG} --Rate0=${RATE0} --Rate1=${RATE1} --Delay0=${DELAY0} --Delay1=${DELAY1} --LossRate=$LOSS" >$LOG1 2>$LOG2

    FILE="scheduler${LOG}-rx.txt"
    cp $FILE "${DIR}${FILE}"
    rm $FILE

    cp $LOG1 "${DIR}${LOG1}"
    rm $LOG1

    cp $LOG2 "${DIR}${LOG2}"
    rm $LOG2

done
