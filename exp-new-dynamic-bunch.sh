
#!/bin/bash


for i in {1..100} #size
do
    ./exp-icc-dynamic0.sh $i
    python3 results-icc/draw.py results-icc/dynamic0-$i
    
done

