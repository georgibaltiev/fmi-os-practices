#!/bin/bash

local_usr=$(mktemp)
prevcloud_usr=$(mktemp)

if [[ -z "${PASSWD}" ]]; then
    PASSWD=/etc/passwd
fi

# local users
cat "${PASSWD}" | awk -F ':' '{$3 > 1000}' | cut -d ':' -f 1 | sort | uniq > "${local_usr}"

exec_path="$(dirpath .)/occ"

# remote users
"${exec_path}" user:list | cut -d ' ' -f 2 | cut -d ':' -f 1 | sort | uniq > "${prevcloud_usr}"

# add non-existent remote users
while read user; do

    "${exec_path}" user:add "${user}"

done < <(comm -23 "${local_usr}" "${prevcloud_usr}")

# disable accounts that do not exist on the remote system
while read user; do

    enabled=$("${exec_path}" user:info "${user}" | grep '^ - enabled: (true|false)$' | sed -E 's/^ - enabled: (true|false)$/\1/g')

    if [[ "${enabled}" == 'true' ]]; then
        "${exec_path}" user:disable "${user}"
    fi
    
done < <(comm -13 "${local_usr}" "${prevcloud_usr}")

# enable disabled accounts that do exist on both systems
while read user; do

    enabled=$("${exec_path}" user:info "${user}" | grep '^ - enabled: (true|false)$' | sed -E 's/^ - enabled: (true|false)$/\1/g')

    if [[ "${enabled}" == 'false' ]]; then
        "${exec_path}" user:enable "${user}"
    fi
    
done < <(comm -12 "${local_usr}" "${prevcloud_usr}")

rm "${local_usr}"
rm "${prevcloud_usr}"
