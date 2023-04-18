
## generate data for figure 9 - 13
echo "...generating data for figure 9-13"
for i in {1..200}
do
    ./exp-wns3-two-path-scheduler.sh $i
    ./exp-wns3-two-path-scheduler-unstable.sh $i
done

## set seed=2
./exp-wns3-two-path-scheduler-flip.sh 2
echo "...figure 9-13 data generated in results-wns3"