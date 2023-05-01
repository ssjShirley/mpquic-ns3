## generate data for figure 7

echo "...generating data for figure 7"
for i in {1..50}
do
    ./exp-wns3-one-path.sh $i
    ./exp-wns3-two-path.sh $i
    ./exp-wns3-four-path.sh $i
done
echo "...figure 7 data generated in results-wns3"

## generate data for figure 8
echo "...generating data for figure 8"
./exp-wns3-two-path-cwnd.sh 0 1
./exp-wns3-two-path-cwnd.sh 1 1
echo "...figure 8 data generated in results-wns3"