
#!/bin/bash

FOLD="eight-$1"
DIR="results-wns3/$FOLD/"
RATE0="10.0"
RATE1="11.0"
DELAY0="50.0"
DELAY1="55.0"
LOSS="0.000000"
# Size=0
Size=5242800
SEED=$1


mkdir ${DIR}
for i in {0..1}
do
    LOG="$i"
    LOG1="scheduler${LOG}-queue.txt"
    LOG2="scheduler${LOG}-flowsum.txt"
    ./waf --run "scratch/wns3-mpquic-eight-path.cc --Seed=$SEED --Size=$Size --SchedulerType=${LOG} --Rate0=${RATE0} --Delay0=${DELAY0} --Rate1=${RATE1} --Delay1=${DELAY1} --LossRate=$LOSS" >$LOG1 2>$LOG2

    FILE="scheduler${LOG}-rx.txt"
    cp $FILE "${DIR}${FILE}"
    rm $FILE

    cp $LOG1 "${DIR}${LOG1}"
    rm $LOG1

    cp $LOG2 "${DIR}${LOG2}"
    rm $LOG2

done

# python3 results-icc/draw.py $DIR