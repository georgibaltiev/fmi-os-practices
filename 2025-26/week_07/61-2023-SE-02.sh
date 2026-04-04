#!/bin/bash

# check if 1 is number

number="${1}"

shift

counter=0
elapsed_time=0

while [ $(echo "${elapsed_time} ${number}" | awk '$1 < $2 {}') ==  1 ]; do
   
    before=$(date +'%s.%N')    
    (${@} &>/dev/null)
    after=$(date +'%s.%N')

    duration=$(echo "${after} - ${before}" | bc)

    elapsed_time=$(echo "${elapsed_time} + ${duration}" | bc)
    counter=$(( "${counter}" + 1 ))
    
done

avg=$(echo "scale=2; ${elapsed_time} / ${counter}" | bc)

rounded=$(echo "scale=2; ${elapsed_time}" | bc)

echo "Ran ${@} for ${rounded} seconds ${counter} times"
echo "${avg}"
