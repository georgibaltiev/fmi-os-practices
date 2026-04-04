#!/bin/bash

# всички триения са с echo (dry-run)

if [[ "${#}" -ne 2 ]]; then
    echo "Invalid amount of params!" 1>&2
    exit 1
fi

if [[ ! -d "${1}" ]]; then
    echo "${1} is not a dir!" 1>&2
    exit 2
fi

if echo "${2}" | grep -qvE '^([1-9][0-9]*|0)$'; then
    echo "${2} is NaN!" 1>&2
    exit 3
fi

# тази проверка не съм сигурен че е нужна за задачата, но не пречи да се направи валидация за вътрешната структура на директорията
if [[ ! -d "${1}/0" || ! -d "${1}/1" || ! -d "${1}/2" || ! -d "${1}/3" ]]; then
    echo "Subdirectory is not in the correct format!" 1>&2
    exit 4
fi

function cleanup_archive() {
    
    local dir=${1}
    local target_usage=${2}
    candidates=$(mktemp)

    while read machine; do
       
        # запазваме всички кандидати за изтриване на конкретната машина
        # първо сортираме по заета памет, за да приоритизираме по-обемните бекъпи, после сортираме по време, за да изтрием по-старите бекъпи
        find "${dir}" -type f -name "${machine}*" -printf '%p %s\n' | sort -t ' ' -r -n -k 2,2  -k 1,1 | tail -n +$(("$(basename ${dir})" + 2)) >> "${candidates}"

    done < <(find "${dir}" -type f -printf '%p\n' | xargs -I{} basename {} | cut -d '-' -f 1,2 | sort | uniq) 

    # за да можем да изберем измежду архивите на всички машини кои са най-подходящи за изтриване, тук сортираме просто по размер 
    while read candidate; do
        
        # правим проверка, за да можем да терминираме процеса ако вече сме паднали под нужното потребление
        if [[ $(df . | tail -n 1 | grep -oE '[1-9][0-9]*\%' | tr -d '%') -lt "${target_usage}" ]]; then
            
            # почистваме счупените символни връзки

            while read symlink; do
                
                if [[ ! -e "${symlink}" ]]; then
                    echo rm "${symlink}"
                fi

            done < <(find -type l "${dir}")

            rm "${candidates}"
            exit 0
        fi

        echo "rm ${candidate}"

    done < <(cat "${candidates}" | sort -t ' ' -nrk 2 | cut -d ' ' -f 1) 

    rm "${candidates}"
}


cleanup_archive "${1}/0" "${2}"

cleanup_archive "${1}/1" "${2}"

cleanup_archive "${1}/2" "${2}"

cleanup_archive "${1}/3" "${2}"
