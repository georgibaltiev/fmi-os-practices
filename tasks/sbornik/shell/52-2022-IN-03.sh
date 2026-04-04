#!/bin/bash

isJava=''
hasJar=''

main_args=''
options=''
filename=''

while [[ "${#}" -gt 0 ]]; do

    if [[ "${1}" == 'java' ]]; then
    
        isJava='true'
    
    elif [[ "${1}" == '-jar' ]]; then
        
        hasJar='true'
    
    elif [[ ! -z "${hasJar}" ]] && [[ -z "${filename}" ]] && echo "${1}" | grep -qE '^[^-]'; then
    
        filename="${1}"
    
    elif [[ ! -z "${hasJar}" ]] && echo "${1}" | grep -qE '^-D[^[:space:]]+=[^[:space:]]+$'; then
        
        options=$(echo "${1} ${options}")
    
    elif [[ -z "${hasJar}" ]] && echo "${1}" | grep -qE '^-D[^[:space:]]+=[^[:space:]]+$'; then
        
        shift
        continue
    
    elif echo "${1}" | grep -qE '^\-'; then
    
        variables=$(echo "${1} ${variables}")
    
    fi

    shift
done


if [[ -z "${isJava}" ]]; then
    echo "Missing java command!" 1>&2
    exit 1
fi

if [[ -z "${hasJar}" ]]; then
    echo "No jar file specified!" 1>&2
    exit 2
fi

echo "java ${options} -jar ${filename} ${variables}"
