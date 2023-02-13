
#!/bin/bash

# cp -r /home/shirley/ns3/quic/results-wns3/* results-quic/


# ./exp-wns3-two-path-cwnd.sh 0 4

# ./exp-wns3-two-path-cwnd.sh 1 4

for i in {1..50}
do
    ./exp-wns3-two-path-cwnd-copy.sh 0 $i
    ./exp-wns3-two-path-cwnd-copy.sh 1 $i

done
