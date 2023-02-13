
#!/bin/bash

FOLD="multi0-$1"
DIR="results-wns3/$FOLD/"
RATE0a="5.0"
RATE0b="5.05"
DELAY0a="50.0"
DELAY0b="50.5"
LOSS="0.000000"
BLambda="200"
BVar="0"
# Size=0
Size=5242800
SEED=$1
Sf=2

mkdir ${DIR}
for i in 0
do
    LOG="$i"
    LOG1="scheduler${LOG}-queue.txt"
    LOG2="scheduler${LOG}-flowsum.txt"
    ./waf --run "scratch/mpquic-multi-path.cc --Seed=$SEED --SubflowNumber=$Sf --Size=$Size --BVar=$BVar --BLambda=$BLambda --SchedulerType=${LOG} --Rate0a=${RATE0a} --Delay0a=${DELAY0a} --Rate0b=${RATE0b} --Delay0b=${DELAY0b} --LossRate=$LOSS" >$LOG1 2>$LOG2

    FILE="scheduler${LOG}-rx.txt"
    cp $FILE "${DIR}${FILE}"
    rm $FILE

    cp $LOG1 "${DIR}${LOG1}"
    rm $LOG1

    cp $LOG2 "${DIR}${LOG2}"
    rm $LOG2

done

# python3 results-icc/draw.py $DIR