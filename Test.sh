#!/bin/bash

# example call
# bash Test.sh

# Constants
mainFolder="Results"
programFolder="Astar"
finalFile="Test"
tempTime="tempTime"
tempTest="tempTest"

pythonCode="Analisis.py"

# clean the remaining files
[ -e $mainFolder ] &&rm -r $mainFolder
[ -e $tempTime ] && rm $tempTime
[ -e $tempTime ] && rm $tempTime
[ -e "LatexTable.txt" ] && rm "LatexTable.txt"

# Create folder if it does not exist
function createIfNotExist()
{
  if ! [ -d $1 ]; then 
    echo "creating" $1 "Folder"
    mkdir $1
  fi
}


function readAndOrganize(){
  results=()
  # echo "read and org"
  while read line; do
    arrLine=(${line//;/ })
    data=${arrLine[1]}
    for ((i=2; i<${#arrLine[@]};i++)); do
    	data=$data" "${arrLine[$i]}
    done
    results[${arrLine[0]}]=$data
  done < $2
  
  for (( si=0; si <$1; si++));do
    echo "${results[si]}" >> $3
  done
}


clean=false
if ! [ -z $1 ]; then
  clean=$1
fi

if $clean; then
  #echo "Clean With Makefile"
  cd $programFolder
  g++ -lpthread  AStar.cpp -o Test
  #make clean
  #make
  cd ..
fi

# run, not the same files !
createIfNotExist $mainFolder

Mode=("Linear" "MThread" "DMThread")
sizes=(10 25 50 75 100 125 150 175 200 225 250)
individuals=(1 2 4 6 8 10 12 14 16)
threadQ=(2 3 4 5 6 7 8 9 10 11 12 13 14 15 16)
totalModes=3
# TODO PUT ALL THE THINGS !
totalTime=20
totalSizes=11
totalThreadConfigurations=15
totalInd=9

for (( modes=1; modes <$totalModes; modes++));do
  echo "$mainFolder"/"${Mode[$modes]}"
  createIfNotExist "$mainFolder"/"${Mode[$modes]}"
  
  for (( sz=0; sz <$totalSizes; sz++));do  
    for (( ind=0; ind <$totalInd; ind++));do  
      # nowComplete=$(date +%s.%N)
      if [[ $modes = 0 ]]; then
        for (( tt=0; tt < $totalTime; tt++));do
          # Program | mode | number Threads | map size | total individuals | debug mode
          now=$(date +%s.%N)
          echo "./$programFolder/$finalFile $modes 1" ${sizes[$sz]} ${individuals[$ind]} "0 0"
          ./$programFolder/$finalFile $modes 1 ${sizes[$sz]} ${individuals[$ind]} 0 0
          rt=$( echo "$(date +%s.%N)-$now" | bc -l)
          echo "$rt">> $mainFolder/${Mode[$modes]}/$tempTime"_"${sizes[$sz]}"_"${individuals[$ind]}
        done
      fi
      if [[ $modes != 0 ]] ;then
        for (( tc=0; tc < $totalThreadConfigurations; tc++));do
          for (( tt=0; tt < $totalTime; tt++));do
            # Program | mode | number Threads | map size | total individuals | debug mode
            now=$(date +%s.%N)
            echo "./$programFolder/$finalFile $modes" ${threadQ[$tc]} ${sizes[$sz]} ${individuals[$ind]} "0 0"
            ./$programFolder/$finalFile $modes ${threadQ[$tc]} ${sizes[$sz]} ${individuals[$ind]} 0 0
            rt=$( echo "$(date +%s.%N)-$now" | bc -l)
            echo "$rt">> $mainFolder/${Mode[$modes]}/$tempTime"_"${sizes[$sz]}"_"${individuals[$ind]}_${threadQ[$tc]}
          done
        done
      fi
    done
  done
done


# Now lest python do the tables !!!
python3 $pythonCode




