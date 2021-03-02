make

echo '=== test start ==='

./mincc 42 > tmp.s
gcc -o tmp tmp.s
./tmp
echo "42?: $?"

./mincc 123 > tmp.s
gcc -o tmp tmp.s
./tmp
echo "123?: $?"

./mincc '1+2' > tmp.s
gcc -o tmp tmp.s
./tmp
echo "3?: $?"

./mincc '34+8' > tmp.s
gcc -o tmp tmp.s
./tmp
echo "42?: $?"

echo '=== test end ==='
