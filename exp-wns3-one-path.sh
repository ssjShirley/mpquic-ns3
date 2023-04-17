
#!/bin/bash

FOLD="one-$1"
DIR="results-wns3/$FOLD/"
RATE0a="5.0"
RATE0b="6.0"
DELAY0a="50.0"
DELAY0b="55.0"
LOSS="0.000000"
Size=5242800
SEED=$1

mkdir ${DIR}

LOG1="scheduler0-queue.txt"
LOG2="scheduler0-flowsum.txt"
./waf --run "scratch/wns3-mpquic-one-path.cc --Seed=$SEED --Size=$Size --SchedulerType=0 --Rate0a=${RATE0a} --Delay0a=${DELAY0a} --Rate0b=${RATE0b} --Delay0b=${DELAY0b}  --LossRate=$LOSS" >$LOG1 2>$LOG2

FILE="scheduler0-rx.txt"
cp $FILE "${DIR}${FILE}"
rm $FILE

cp $LOG1 "${DIR}${LOG1}"
rm $LOG1

cp $LOG2 "${DIR}${LOG2}"
rm $LOG2
