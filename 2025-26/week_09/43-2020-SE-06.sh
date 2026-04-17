#!/bin/bash

if [[ "${#}" -ne 3 ]]; then
	echo 'Insufficient amount of arguments' 1>&2
	exit 1
fi

if [[ ! -f "${1}" ]]; then
	echo 'First applied parameter should be a file!' 1>&2
	exit 2
fi

paramregex='[a-zA-Z0-9_]+'

if ! echo "${2}" | grep -qE "^${paramregex}$"; then
	echo 'Applied parameter is not in the correct format!' 1>&2
	exit 3
fi

if ! echo "${3}" | grep -qE "^${paramregex}$"; then
	echo 'Applied value is not in the correct format!' 1>&2
	exit 4
fi

found_line=$(cat "${1}" | grep -E "^[ ]*${2}[ ]*=[ ]*${paramregex}")

if [[ -n "${found_line}" ]]; then

    # извличаме и стойността на ключа
    value=$(echo "${found_line}" | sed -E "s/^[ ]*${2}[ ]*=[ ]*(${paramregex})/\1/g")

    if [[ "${value}" == "${3}" ]]; then
        exit 0
    fi

    # този sed ни променя реда, като му добавя един коментар най-отзад и конкатенира нов ред с edit-натата информация
	sed  -iE "s/${found_line}/# & # edited at $(date) by $(whoami)\n${2} = ${3} # added at $(date) by $(whoami)/g" "${1}"

    exit 0
fi

echo "${2} = ${3} # added at $(date) by $(whoami)" >> "${1}"
