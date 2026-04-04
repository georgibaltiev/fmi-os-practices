
mkdir dir1

chmod 700 или chmod u=rwx,g=,o= dir1

---

mkdir -p dir1/a

chmod 542 или chmod u=rx,g=r,o=w dir1/a

---

mkdir -p dir1/b

chmod 713 или chmod u=rwx,g=x,o=wx dir1/b

---

touch dir1/a/f1

chmod 400 или chmod u=r,g=,o= dir1/a/f1

---

touch dir1/a/f2

chmod 352 или chmod u=wx,g=rx,o=w dir1/a/f2 

---

touch dir1/b/f3

chmod 644 или chmod u=rw,g=r,o=r dir1/b/f3

