
cp /etc/passwd ~/my_passwd

В зависимост от това къде се намира вашата home директория (realpath ~), създаваме символна връзка към абсолютния път до файла my_passwd

ln -s ~/my_passwd или (абсолютен път до ~)/my_passwd passwd_symlink
