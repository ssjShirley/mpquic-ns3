
#!/bin/bash
for i in 2 6 10  #rate0
do
    for k in 40 45 50  #rate1
    do
        ./exp-complete-time-mab.sh 40 $i 80 $k 20
    done 
done


# ./exp-complete-time-mab.sh 40 2 80 40 20