## experiment data generating

for i in {1..300}
do
    ./exp-dynamic0.sh $i
    ./exp-dynamic0_competing.sh $i
    ./exp-dynamic1.sh $i
    ./exp-dynamic1_competing.sh $i
    ./exp-dynamic2.sh $i
    ./exp-dynamic2_competing.sh $i
done

for i in {1..200}
do
    ./exp-four-path.sh $i
done