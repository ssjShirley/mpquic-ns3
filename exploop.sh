
#!/bin/bash

# for i in 80
# for i in 81 82 83 84 85 86 87 88 89 90
# do
#     for j in 8 16 24 32 40 48 56 64 72
#         do ./exp-nbg-ndy.sh $i $j
#     done
# done

# for i in 10 20 30 40 50 #rate
# do
#     for j in 20 40 60 80 100 #delay
#     do
#         ./exp-nbg-ndy-test.sh 80 $i $j
#     done
# done

# for i in 10 20 30 40 50 #rate
# do
#     for j in 20 40 60 80 100 #delay
#     do
#         ./exp-nbg-ndy.sh 80 $i $j
#     done
# done


for j in 45 46 47 48 49 50 51 52 53 54 55  #size
do
    ./exp-pretest.sh $j 10 10
    ./exp-pretest.sh $j 100 5
    ./exp-pretest.sh $j 200 2
done

