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

echo '=== test end ==='
