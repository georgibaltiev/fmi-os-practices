#!/bin/bash

if [[ "${#}" -ne 1 ]]; then
    echo "Insufficient amount of params!" >&2
    exit 1
fi

if echo "${1}" | grep -qvE '^0|([1-9][0-9]*)$'; then
    echo "${1} should be a non-negative integer!" >&2
    exit 2
fi

accords=$(mktemp)
transposed=$(mktemp)

echo -e "Ab\nA\nBb\nB\nC\nDb\nD\nEb\nE\nF\nGb\nG" > "${accords}"

# good point on that, трябва да разделим процентно на 12 за да успеем да прихванем случаите където ротацията се случва с повече от 12 скока
num_shift=$(echo "${1} % 12" | bc)

tail -n +$(( "${num_shift}" + 1 )) "${accords}" > "${transposed}"
head -n "${num_shift}" "${accords}" >> "${transposed}"

# правим си mapping, който съдържа оригиналния акорд, акорда който трябва да се получи след ротацията, както и число, с което да направим анотацията за да избегнем презаписване
# тук правя промяна, и вместо да запазвам mapping-а в tempfile, го правя в променлива, тъй като по-надолу имаме безкраен цикъл който чете от STDIN и се прекъсва единствено със SIGINT сигнал, тоест няма как да затрия този файл
mapping=$(paste "${accords}" "${transposed}" <(seq 1 12))

rm "${transposed}"
rm "${accords}"

while read line; do

    # замяна <оригинален акорд> -> <уникален номер><оригинален акорд>
    while read original transposed id; do

        line=$(echo "${line}" | sed -E "s/\[${original}(.*)\]/[${id}${original}\1]/g")

    # пак mapping-а го слагам в process substitution вместо да чета от файл
    done < <(echo "${mapping}")

    # замяна <уникален номер><оригинален акорд> -> <нов акорд>
    while read original transposed id; do

        line=$(echo "${line}" | sed -E "s/\[${id}${original}(.*)\]/[${transposed}\1]/g")
    
    done < <(echo "${mapping}")

    echo "${line}"

done
