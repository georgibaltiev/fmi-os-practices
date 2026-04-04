#!/bin/bash

if [[ "${#}" -ne 1 ]]; then
    echo "Invalid amount of params!" 1>&2
    exit 1
fi

if [[ ! -d "${1}" ]]; then
    echo "${1} is not a valid directory!" 1>&2
    exit 2
fi

# проверки дали имаме foo.conf и foo.pwd
if [[ ! -e "${1}/foo.conf" ]]; then
    echo "${1}/foo.conf not found!" 1>& 2
    exit 3
fi

if [[ ! -e "${1}/foo.pwd" ]]; then
    echo "${1}/foo.pwd not found!" 1>&2
    exit 4
fi

if [[ ! -e "${1}/cfg" && ! -d "${1}/cfg" ]]; then
    echo "${1}/cfg is not a dir!" 1>&2
    exit 5
fi

if [[ ! -e "${1}/validate.sh" ]] && [[ ! -f "${1}/validate.sh" ]]; then
    echo "${1} does not have a validating script!" 1>&2
    exit 6
fi

new_conf=$(mktemp)

while read conf_file; do
    
    err_file=$(mktemp)

    ${1}/validate.sh "${conf_file}" > "${err_file}"
   
    # успешните user-и влизат в новата конфигурация
    if [[ "${?}" -eq 0 ]]; then
        cat "${conf_file}" >> "${new_conf}"
    
    user="$(basename ${conf_file} | cut -d '.' -f 1)" 

    # логика за да модифицираме .pwd файла
    if grep -q "${user}" "${1}/foo.pwd"; then
        continue
    else
        password=$(pwgen -1)
        echo "${user}:${password}"
        echo "${user}:$(mkpasswd ${password})" >> "${1}/foo.pwd"
    fi

    # fail-налите user-и отиват на stderr с модификацията на output-а 
    elif [[ "${?}" -eq 1 ]]; then
        cat "${err_file}" | awk -v user="${conf_file}:" '{print user, $0}' 1>&2
    # тук assume-вам че трябва незабавно скрипта да прекрати изпълнението си
    else
        echo "Validating ${conf_file} failed unexpectedly!" 1>& 2
        rm "${new_conf}" "${err_file}"
        exit 3
    fi

    rm "${err_file}"

done < <(find "${1}/cfg" -type f -name '*.cfg')

cat "${new_conf}" > "${1}/foo.conf"

rm "${new_conf}"
