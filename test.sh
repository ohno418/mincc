make
./mincc > tmp.s
gcc -S -o tmp tmp.s
./tmp
echo $?
