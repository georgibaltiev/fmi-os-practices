#!/bin/bash

#if [[ "$(whoami)" != 'root' ]]; then
#    echo "Non-root permission denied!" 1>&2
#    exit 1
#fi

snapshot=$(mktemp)

# Извикваме ps само веднъж, за да направим един snaphot на процесите и техните данни. Причината за това е че има възможност да има неконсистентност в данните между различните извиквания на ps
ps -e -o user=,rss=,pid= > "${snapshot}"

while read user; do
    
    # изчисляваме средното потребление на памет за конкретния user 
    avg_rss=$(cat "${snapshot}" | awk -v usr="${user}" 'BEGIN {count = 0; sum = 0} $1 == usr {count++; sum += $2} END {print sum / count}')
    
    while read pid; do
        
        # тук правим само dry-run, за да не се налага да извикваме истинската команда
        echo "kill ${pid}"

    # създаваме списък от процесите на конкретния user, които имат поне двойно повече потребление на физическа памет от средното им
    done < <(cat "${snapshot}" | awk -v usr="${user}" '$1 == usr' | awk -v avg="${avg_rss}" '$2 > 2 * avg {print $3}')

done < <(cat "${snapshot}" | awk '{print $1}' | sort | uniq )

# ЗАДЪЛЖИТЕЛНО
rm "${snapshot}"
