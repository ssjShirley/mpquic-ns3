
#!/bin/bash
for i in 2 4 6 8 10  #rate0
do
    for k in 40 42 44 46 48 50  #rate1
    do
        ./exp-complete-time.sh 80 $i $k 80 20
    done 
done

