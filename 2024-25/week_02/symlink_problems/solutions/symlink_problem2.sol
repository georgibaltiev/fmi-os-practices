Приемайки, че дървото на директориите вече е създадено

За всяка двойка команда приемаме, че в началото се намираме в директорията dir/..

---

cd dir/a/b/d
ln -s ../../target.txt link_d

---

cd dir/a/b
ln -s target.txt link_b

---

cd dir/a
ln -s b/target.txt link_a

cd dir/c
ln -s ../a/b/target.txt link_c
