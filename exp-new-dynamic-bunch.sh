
#!/bin/bash


for i in {1..100} #size
do
    ./exp-icc-dynamic1.sh $i
    python3 results-icc/draw.py results-icc/peedynamic1-$i
    
done

for i in {1..100} #size
do
    ./exp-icc-dynamic2.sh $i
    python3 results-icc/draw.py results-icc/peedynamic2-$i
    
done



