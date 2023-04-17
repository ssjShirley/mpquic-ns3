## generate data for figure 7
for i in {1..50}
do
    ./exp-wns3-one-path.sh $i
    ./exp-wns3-two-path.sh $i
    ./exp-wns3-four-path.sh $i
done

## generate data for figure 8
./exp-wns3-two-path-cwnd.sh 0 1
./exp-wns3-two-path-cwnd.sh 1 1