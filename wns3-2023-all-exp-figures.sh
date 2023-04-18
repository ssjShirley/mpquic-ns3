#!/bin/bash

## generate all required data
echo "...generate all required data"
./figure7-8-data-generate.sh
./figure9-13-data-generate.sh

## draw the figures
echo "...draw the figures"
cd results-wns3

python3 figure7a.py
echo "...figure7a saved in results-wns3/scalable_comtime.png"

python3 figure7b.py
echo "...figure7b saved in results-wns3/ins_throughput_scalable.png"

python3 figure8a.py
echo "...figure8a saved in results-wns3/cwnd_newreno.png"

python3 figure8b.py
echo "...figure8b saved in results-wns3/cwnd_OLIA.png"

python3 figure9a.py
echo "...figure9a saved in results-wns3/comTime_scheduler.png"

python3 figure9b.py
echo "...figure9b saved in results-wns3/ins_throughput_scheduler.png"

python3 figure10a.py
echo "...figure10a saved in results-wns3/comTime_scheduler_unstable.png"

python3 figure10b.py
echo "...figure10b saved in results-wns3/ins_throughput_scheduler_unstable.png"

python3 figure11.py
echo "...figure11 saved in results-wns3/received_bytes_scheduler0.png"
echo "...figure11 saved in results-wns3/received_bytes_scheduler1.png"
echo "...figure11 saved in results-wns3/received_bytes_scheduler2.png"
echo "...figure11 saved in results-wns3/received_bytes_scheduler3.png"
echo "...figure11 saved in results-wns3/received_bytes_scheduler4.png"

python3 figure12.py
echo "...figure12 saved in results-wns3/received_bytes_scheduler_flip0.png"
echo "...figure12 saved in results-wns3/received_bytes_scheduler_flip1.png"
echo "...figure12 saved in results-wns3/received_bytes_scheduler_flip2.png"
echo "...figure12 saved in results-wns3/received_bytes_scheduler_flip3.png"
echo "...figure12 saved in results-wns3/received_bytes_scheduler_flip4.png"

python3 figure13.py
echo "...figure13 saved in results-wns3/received_bytes_scheduler_unstable0.png"
echo "...figure13 saved in results-wns3/received_bytes_scheduler_unstable1.png"
echo "...figure13 saved in results-wns3/received_bytes_scheduler_unstable2.png"
echo "...figure13 saved in results-wns3/received_bytes_scheduler_unstable3.png"
echo "...figure13 saved in results-wns3/received_bytes_scheduler_unstable4.png"