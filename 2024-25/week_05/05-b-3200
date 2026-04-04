#!/bin/bash

# Да се напише shell скрипт, който приканва потребителя да въведе пълното име на директория и извежда на стандартния изход подходящо съобщение за броя на всички файлове и всички директории в нея.

read -p "Enter directory" dir_name

amount_of_files=$(find "${dir_name}" -type f 2> /dev/null | wc -l)
amount_of_dirs=$(find "${dir_name}" -type d 2> /dev/null | wc -l)

echo "The amount of files is ${amount_of_files}"
echo "The amount of dirs is ${amount_of_dirs}"
