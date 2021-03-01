make

echo '=== test start ==='

./mincc > tmp.s
gcc -o tmp tmp.s
./tmp
echo "42?: $?"

echo '=== test end ==='
