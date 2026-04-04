#!/bin/bash

if [[ "${#}" -ne 1 ]]; then
    echo "Invalid amount of parameters" 1>&2
    exit 1
fi

if [[ "$(whoami)" != root ]]; then
    echo "Permission denied" 1>&2
    exit 2
fi

while read dir mode bits; do
    
    updated_mode=''
    if [[ "${mode}" == R ]]; then
        updated_mode=''
    elif [[ "${mode}" == A ]]; then
        updated_mode='/'
    elif [[ "${mode}" == T ]]; then
        updated_mode='-'
    else 
        echo "${mode} is not valid!" 1>&2
        exit 1
    fi

    # условията с промяна на права, все едно файлът е създаден с umask ... не съм сигурен дали се интерпретират по този начин, но го оставям така
    find ${dir} -mindepth 1 -type f -perm ${updated_mode}${bits} -exec echo chmod 664 '{}' ';'
    find ${dir} -mindepth 1 -type d -perm ${updated_mode}${bits} -exec echo chmod 755 '{}' ';'

done < "${1}"
