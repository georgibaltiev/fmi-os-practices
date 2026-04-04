
umask 077
mkdir d1

--- 

umask 135
touch d1/a

---

umask 513
touch d1/b

---

umask 133
touch d1/c
