#!/bin/bash

if [[ "${#}" -ne 2 ]]; then
    echo "Invalid amount of parameters" 1>&2
    exit 1
fi

# модифицирал съм regex-а за да приема и отрицателни числа

if echo "${1}" | grep -qvE '^([-]?[1-9][0-9]*|0)$'; then
    echo "${1} NaN" 1>&2
    exit 2
fi

if echo "${2}" | grep -qvE '^([-]?[1-9][0-9]*|0)$'; then
    echo "${2} NaN" 1>&2
    exit 2
fi

if [[ "${1}" -gt "${2}" ]]; then
    echo "${1} is bigger than ${2}!" 1>&2
    exit 3
fi

while read file; do

    lines=$(cat "${file}" | wc -l)

    if [[ "${lines}" -lt "${1}" ]]; then
        
        mkdir -p a
        cp "${file}" a

    # тук има корекция от недоглеждане на условието, вторият тест е -le вместо -lt
    elif [[ "${lines}" -ge "${1}" && "${lines}" -le "${2}" ]]; then
 
        mkdir -p b
        cp "${file}" b 

    else

        mkdir -p c
        cp "${file}" c 

    fi

# задачата предполага, че в текущата директория има само обикновени файлове, и няма поддиректории
done < <(find . -mindepth 1 -maxdepth 1 -type f)
