#!/bin/bash

hasJar=''
options=''
filename=''
variables=''

# NB: премахнах проверката дали подаваме командата java като входен параметър, тъй като е безсмислена
# iwojima.sh трябва да приема само входните параметри, които после да бъдат подадени на java командата

while [[ "${#}" -gt 0 ]]; do
    
    if [[ "${1}" == '-jar' ]]; then
        
        hasJar='true'
    
    # приемаме, че първия позиционен параметър след -jar флага, който НЕ започва с тире е името на JAR файла.
    elif [[ -n "${hasJar}" ]] && [[ -z "${filename}" ]] && echo "${1}" | grep -qE '^[^-]'; then
    
        filename="${1}"
    
    # опциите, които са след -jar флага и са от формата -Dproperty=value могат да се добавят към списъка с опции
    elif [[ -n "${hasJar}" ]] && [[ -n "${filename}" ]] && echo "${1}" | grep -qE '^-D[^[:space:]]+=[^[:space:]]+$'; then
        
        options=$(echo "${options} ${1}")
    
    # опциите, които са преди -jar флага и са от формата -Dproperty=value НЕ могат да се добавят към списъка с опции
    elif [[ -z "${hasJar}" ]] && echo "${1}" | grep -qE '^-D[^[:space:]]+=[^[:space:]]+$'; then 
        shift
        continue
    # ако вече сме намерили filename-а, добавяме параметъра към променливите
    elif [[ -n "${hasJar}" ]] && [[ -n "${filename}" ]]; then
    
        variables=$(echo "${variables} ${1}")
    
    # последния случай са опции, които нe са от формата -Dproperty=value, те се добавят винаги към опциите
    else
	options=$(echo "${options} ${1}")
    fi

    shift
done

if [[ -z "${hasJar}" ]]; then
    echo "No jar file specified!" 1>&2
    exit 2
fi

echo "java ${options} -jar ${filename} ${variables}"
