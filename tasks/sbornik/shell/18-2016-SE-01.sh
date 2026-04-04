#!/bin/bash

if [[ "$(whoami)" != root ]]; then
    echo "Permission denied!" 1>&2
    exit 1
fi

if echo "${1}" | grep -qE '^([1-9][0-9]*|0)$'; then
    echo "${1} must be a positive integer!" 1>&2
    exit 2
fi

while read user; do
    
    snapshot=$(mktemp)

    ps -u "${user}" -o pid=,rss= > "${snapshot}"

    # сумираме на всеки потребител потреблението на памет
    sum_rss=$(cat "${snapshot}" | awk 'BEGIN {sum = 0} {sum += $2} END {print sum}')

    if [[ "${sum_rss}" -gt "${1}" ]]; then
        
        target_pid=$(cat "${snapshot}" | tr -s ' ' | sed -E 's/^[[:space:]]*//' | sort -t ' ' -nrk 2  | head -n 1 | cut -d ' ' -f 1)

        # изпратим подходящи сигнали за прекратяването на процеса
        # по-учтива имплементация на прекратяването на процес, тук му оставяме време сам да се прекрати, преди да опитаме да го убием директно
        kill "${target_pid}"
        sleep 1
        # възможно е вече да сме убили процеса който сме искали, затова евентуално при съобщение за грешка тук го препращаме към /dev/null
        kill -9 "${target_pid}" 2>/dev/null

    fi

    rm "${snapshot}"

done < <(ps -e -o user= | sort | uniq) 
