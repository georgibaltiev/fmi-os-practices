#!/bin/bash

# example pwgen key
annotate=Go6cho_s

params=$(mktemp)
files=$(mktemp)

while [[ "${#}" -gt 0 ]]; do
    
    if echo "${1}" | grep -qE '^-R[^=]*=[^=]*$'; then
        
        echo "${1}" | sed -E 's/^-R([^=]*)=([^=]*)$/\1 \2/' >> "${params}"

    else
        
        echo "${1}" >> "${files}"

    fi

shift
done


while read file; do

    if [[ ! -f "${file}" ]]; then
        echo "${file} is not a file!" 1>&2
        continue
    fi

    # съобразяваме че трябва да направим смяната само веднъж за всяка дума, тоест първо анотираме всички думи-кандидати за смяна, и после чак извършваме подмяната им
    while read key value; do
        
        sed -iE "/^[^#]/ s/\b${key}\b/${annotate}${key}/g" "${file}"

    done < "${params}"

    while read key value; do
        
        sed -iE "/^[^#]/ s/\b${annotate}${key}\b/${value}/g" "${file}"

    done < "${params}"

done < "${files}"

rm "${params}"
rm "${files}"
