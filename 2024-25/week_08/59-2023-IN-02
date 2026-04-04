#!/bin/bash


function parse_hash() {

    local list="${1}"
    local hash="${2}"

    copies=$(mktemp)
    
    # запазваме си във временен файл всички файлове с техните inode-ове, които имат конкретната хеш-сума
    cat "${list}" | grep -E "${hash}\$" | cut -d ' ' -f 2 | sort | uniq > "${copies}"


    # тази проверка служи, за да видим дали сме в случая където имаме само отделни файлове които да имат един и същ хеш, т.е. нямаме никакви групи от hardlinks
    if [[ $(cat "${list}" | grep -E "${hash}\$" | wc -l) -eq $(cat "${copies}" | sort | uniq | wc -l) ]]; then
        cat "${list}" | grep -E "${hash}\$" | cut -d ' ' -f 1 | tail -n +2 | xargs -I{} echo rm {}
        return
    fi

    # за всеки прочетен inode избираме 1 от файловете който сочи към него и го махаме
    while read inode; do
        
        echo rm $(cat "${list}" | cut -d ' ' -f 1,2 | grep -E "${inode}\$" | cut -d ' ' -f 1 | head -n 1) 

    done < "${copies}"

    rm "${copies}"

}

if [[ "${#}" -ne 1 ]]; then
    echo 'Invalid amount of params!' 1>&2
    exit 1
fi

if [[ ! -d "${1}" ]]; then
    echo "${1} is not a directory!" 1>&2
    exit 2
fi

list=$(mktemp)

# правим си регистър на всички файлове с техните имена, inode-ове и хеш-суми
while read filename inode; do
    
    echo "${filename} ${inode} $(sha256sum ${filename} | cut -d ' ' -f 1)" >> "${list}"

done < <(find "${1}" -type f -printf '%p %i\n')

# за всеки уникален хеш в директорията извършваме изтриването което е необходимо
while read hash; do
    
    parse_hash "${list}" "${hash}"

done < <(cat "${list}" | cut -d ' ' -f 3 | sort | uniq)

rm "${list}"
