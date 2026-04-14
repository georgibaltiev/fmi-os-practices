#!/bin/bash

function compile() {
    local bakefile="${1}"
    local filename="${2}"

    # проверяваме дали файла има инструкция за компилиране
    if cat "${bakefile}" | grep -qE "^${filename}"; then
        
        line=$(cat "${bakefile}" | grep -E "^${filename}")

        dep=$(echo "${line}" | cut -d ':' -f 2)
        comm=$(echo "${line}" | cut -d ':' -f 3)

        
        for de in ${dep}; do
            
            # компилираме файла, само някое негово dependency е по-ново
            if [[ "${de}" -nt "${filename}" ]]; then
                compile "${bakefile}" "${de}"
                return
            fi

        done

        # тук може да се подходи по много начини с извикването на командата
        # eval е за предпочитане, тъй като успява да resolve-не pipe redirection оператора
        eval "${comm}"

    else
        # ако файла няма инструкция за компилиране

        if [[ -f "${filename}" ]]; then
            return
        else 
            echo "${filename} is not a file!" 1>&2
            exit 1
        fi

    fi
}

if [[ ! -f "${1}" ]]; then
    echo "${1} is not bakefile" 1>&2
    exit 1
fi

compile "${1}" "${2}"
