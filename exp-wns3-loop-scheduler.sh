
#!/bin/bash

# cp -r /home/shirley/ns3/quic/results-wns3/* results-quic/

for j in {1..200}  #size
do
    # ./exp-wns3-scheduler.sh $j
    ./exp-wns3-two-path-scheduler.sh $j
done

