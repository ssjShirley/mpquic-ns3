
#!/bin/bash

# cp -r /home/shirley/ns3/quic/results-wns3/* results-quic/

for j in {1..50}  #size
do
    # ./exp-wns3-one-path.sh $j
    # ./exp-wns3-two-path.sh $j
    ./exp-wns3-four-path.sh $j
    ./exp-wns3-eight-path.sh $j
done

