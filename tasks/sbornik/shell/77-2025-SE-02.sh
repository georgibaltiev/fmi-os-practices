#!/bin/bash

function start_service() {
   
    while read filename; do

        svc_name=$(cat "${filename}" | grep -E '^name: .*$' | cut -d ':' -f 2 | sed -E 's/[ ]*([^ ]+)/\1/g')
        pidfile=$(cat "${filename}" | grep -E '^pidfile: .*$' | cut -d ':' -f 2 | sed -E 's/[ ]*([^ ]+)/\1/g')
        outfile=$(cat "${filename}" | grep -E '^outfile: .*$' | cut -d ':' -f 2 | sed -E 's/[ ]*([^ ]+)/\1/g')
        comm=$(cat "${filename}" | grep -E '^comm: .*$' | cut -d ':' -f 2)
    
    # we check if the process has already started
    if [[ "${svc_name}" != "${1}" ]]; then
        continue
    fi

    if [[ -e "${pidfile}" && -n $(ps -e -o pid= | grep -E "^$(cat ${pidfile})\$") ]]; then
        exit 0
    fi

    eval "${comm} &> ${outfile} &"
    echo "${!}" > "${pidfile}"
    echo "My job here is done"
    exit 0 

    done < <(find "${SVC_DIR}" -type f)

    echo "service ${1} not found!" >&2
    exit 4
}

function stop_service() {
    while read filename; do

    # we assume that all files will be within that format
        svc_name=$(cat "${filename}" | grep -E '^name: .*$' | cut -d ' ' -f 2)
        pidfile=$(cat "${filename}" | grep -E '^pidfile: .*$' | cut -d ' ' -f 2)
    
    # we check if the process has already started
    if [[ "${svc_name}" != "${1}" ]]; then
        continue
    fi

    kill $(cat "${pidfile}") 2>/dev/null

    done < <(find "${SVC_DIR}" -type f)
}

function cleanup() {
    while read filename; do

    # we assume that all files will be within that format
        pidfile=$(cat "${filename}" | grep -E '^pidfile: .*$' | cut -d ':' -f 2 | sed -E 's/[ ]*([^ ]+)/\1/g')

        if [[ -e "${pidfile}" && -z $(ps -e -o pid= | grep $(cat "${pidfile}")) ]]; then
            rm "${pidfile}"
        fi

    done < <(find "${SVC_DIR}" -type f)
}

function running() {

    while read filename; do

    # we assume that all files will be within that format
        svc_name=$(cat "${filename}" | grep -E "^name: .*\$" | cut -d ':' -f 2 | sed -E 's/[ ]*([^ ]+)/\1/g')
        pidfile=$(cat "${filename}" | grep -E "^pidfile: .*\$" | cut -d ':' -f 2 | sed -E 's/[ ]*([^ ]+)/\1/g')

        if [[ -f "${pidfile}" && -n $(ps -e -o pid= | grep -E "^[ ]*$(cat ${pidfile})[ ]*\$") ]]; then
            echo "${svc_name}"
        fi

    done < <(find "${SVC_DIR}" -type f)
}

if ! env | grep -qE '^SVC_DIR=(.*)$'; then
    echo "No env variable SVC_DIR set!" >&2
    exit 1
fi

if [[ ! -d "${SVC_DIR}" ]]; then
    echo "${SVC_DIR} is not a dir!" >&2
    exit 2
fi

if [[ "${#}" -lt 1 ]]; then
    echo "Expected at least one command!" >&2
    exit 3
fi

case "${1}" in
    "start")
        if [[ -z "${2}" ]]; then
            echo "Expected a service name!" >&2
            exit 4
        fi
        start_service "${2}" 
        exit 0
    ;;
    "stop")
        if [[ -z "${2}" ]]; then
            echo "Expected a service name!" >&2
            exit 5
        fi
        stop_service "${2}"
        exit 0
    ;;
    "running")
        if [[ "${#}" -gt 1 ]]; then
            echo "params!" >&2
            exit 6
        fi
        running | sort       
        exit 0
    ;;
    "cleanup")
        if [[ "${#}" -gt 1 ]]; then
            echo "params!" >&2
            exit 7
        fi
        cleanup
        exit 0
    ;;
esac
