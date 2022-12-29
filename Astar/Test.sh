#!/bin/bash


[[ $1 = 1 ]] && clear && g++ -lpthread  AStar.cpp -o Test


# g++ -fPIC -lpthread  UglyAStar.cpp -o Test
now=$(date +%s.%N)
time ./Test $2 $3 $4 $5 $6 $7
rt=$( echo "$(date +%s.%N)-$now" | bc -l)
echo $rt


#for (( a=0; a <20; a++));do
#	./Test 2 2 50 1 0 0
#done