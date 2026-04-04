#!/bin/bash

if [[  "${#}" -ne 2 ]] ; then
    echo "Invalid amount of params!" 1>&2
    exit 1
fi

if [[ ! -d "${2}" ]]; then
    echo "${2} is not a directory!"
    exit 2
fi

while read filename; do

    while read word; do
        
        censor=$(echo "${word}" | tr '[A-Za-z_]' '*')
        sed -iE "s/\b${word}\b/${censor}/g" "${filename}" 

    done < "${1}"

done < <(find "${2}" -type f -name '*.txt')
