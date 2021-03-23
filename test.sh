#!/bin/bash

assert() {
  input="$1"
  expected="$2"

  ./mincc "$input" > tmp.s
  gcc -o tmp tmp.s
  ./tmp
  actual="$?"

  if [ "$actual" == "$expected" ]
    then
      echo "$input => $actual"
    else
      echo "$input => $expected expected, but got $actual"
      exit 1
  fi
}

assert 'int main() { return 42; }' 42
assert 'int main() { return 123; }' 123

assert 'int main() { return 1+2; }' 3
assert 'int main() { return 34+8; }' 42

assert 'int main() { return 1 +2; }' 3
assert 'int main() { return   1+ 2 ; }' 3

assert 'int main() { return 1+2+3; }' 6

assert 'int main() { return 6-2; }' 4
assert 'int main() { return 6+2-3; }' 5

assert 'int main() { return 6*2; }' 12
assert 'int main() { return 6*2-2; }' 10
assert 'int main() { return 2+6*2; }' 14
assert 'int main() { return 2+6*2-3*2; }' 8

assert 'int main() { return 6/2; }' 3
assert 'int main() { return 6/2-1; }' 2
assert 'int main() { return 1+6/2; }' 4
assert 'int main() { return 6/2*3; }' 9
assert 'int main() { return 2*6/2; }' 6

assert 'int main() { return 3==4; }' 0
assert 'int main() { return 3==3; }' 1

assert 'int main() { return 3!=4; }' 1
assert 'int main() { return 3!=3; }' 0

assert 'int main() { return 3<4; }' 1
assert 'int main() { return 3<5; }' 1
assert 'int main() { return 3<2; }' 0
assert 'int main() { return 3<3; }' 0
assert 'int main() { return 3>4; }' 0
assert 'int main() { return 3>5; }' 0
assert 'int main() { return 3>2; }' 1
assert 'int main() { return 3>3; }' 0

assert 'int main() { 1; 2; return 3; }' 3

assert 'int main() { int a; a=42; return a; }' 42
assert 'int main() { int a=42; return a; }' 42
assert 'int main() { int a=42; return a+3; }' 45
assert 'int main() { int a=42; int z=2; return a+z; }' 44

assert 'int main() { int var=34; return var; }' 34
assert 'int main() { int foo=12; int var=34; return foo+var; }' 46
assert 'int main() { int foo=12; int faa=34; return foo+faa; }' 46

assert 'int main() { 1; return 2; return 3; }' 2

assert 'int main() { { return 42; } }' 42
assert 'int main() { { int foo=12; return foo; } }' 12
assert 'int main() { { int foo=12; int faa=34; return foo+faa; } }' 46
assert 'int main() { { 1; return 2; return 3; } }' 2

assert 'int main() { ;; return 5; }' 5

assert 'int main() { if (1) return 3; return 4; }' 3
assert 'int main() { if (0) return 3; return 4; }' 4
assert 'int main() { if (1) { return 3; } return 4; }' 3
assert 'int main() { if (0) { return 3; } return 4; }' 4
assert 'int main() { if (1-1) return 3; return 4; }' 4
assert 'int main() { if (3-2) return 3; return 4; }' 3
assert 'int main() { if (1) { if (1) return 42; return 12; } return 2; }' 42
assert 'int main() { if (1) { if (0) return 42; return 12; } return 2; }' 12
assert 'int main() { if (0) { if (1) return 42; return 12; } return 2; }' 2
assert 'int main() { if (1) { if (1) return 42; return 12; } if (0) return 123; return 2; }' 42
assert 'int main() { if (0) { if (1) return 42; return 12; } if (1) return 123; return 2; }' 123
assert 'int main() { if (0) { if (1) return 42; return 12; } if (0) return 123; return 2; }' 2

assert 'int main() { if (1) return 3; else return 4; return 5; }' 3
assert 'int main() { if (0) return 3; else return 4; return 5; }' 4
assert 'int main() { if (1) { return 3; } else { return 4; } return 5; }' 3
assert 'int main() { if (0) { return 3; } else { return 4; } return 5; }' 4

assert 'int main() { int i=0; for (; i<5; i=i+1) ; return i; }' 5
assert 'int main() { int i=0; int j=1; for (; i<3; i=i+1) j = j * 2; return j; }' 8
assert 'int main() { int i=0; int j=1; for (i=1; i<3; i=i+1) j = j * 2; return j; }' 4

assert 'int ret3() { return 3; } int main() { return ret3(); }' 3
assert 'int add2(int x, int y) { return x+y; } int main() { return add2(4,7); }' 11
assert 'int sub2(int x, int y) { return x-y; } int main() { return sub2(7,4); }' 3
assert 'int fib(int n) { if (n==0) return n; if (n==1) return n; return fib(n-2) + fib(n-1); } int main() { return fib(10); }' 55

assert 'int main() { int a=3; int *p=&a; return *p; }' 3
assert 'int main() { int a=4; return *&a; }' 4
assert 'int main() { int a=5; int *b=&a; int **c=&b; return **c; }' 5
assert 'int main() { int a=6; int *b=&a; *b=7; return a; }' 7

assert 'int main() { return 3+4*5; }' 23
assert 'int main() { return (3+4)*5; }' 35
assert 'int main() { return 3+(4*5); }' 23

assert 'int main() { int a=2; int b=3; return *(&a+1); }' 3
assert 'int main() { int a=4; int b=5; return *(&b-1); }' 4
assert 'int main() { int a=6; int b=7; return *(&b-2+1); }' 6
assert 'int main() { int a=8; int b=9; int c=10; return *(&a+1); }' 9
assert 'int main() { int a=8; int b=9; int c=10; return *(&a+1+1); }' 10

assert 'int main() { return sizeof(2); }' 8
assert 'int main() { int x; return sizeof(x); }' 8
assert 'int main() { int x=4; return sizeof(x); }' 8
assert 'int main() { int x=5; int *p=&x; return sizeof(p); }' 8
assert 'int main() { int a=6; int *b=&a; int **c=&b; return sizeof(c); }' 8

assert 'int main() { int a[2]; return sizeof(a); }' 16
assert 'int main() { int a[3]; *a=2; *(a+1)=3; *(a+2)=4; return *a; }' 2
assert 'int main() { int a[3]; *a=2; *(a+1)=3; *(a+2)=4; return *(a+1); }' 3
assert 'int main() { int a[3]; *a=2; *(a+1)=3; *(a+2)=4; return *(a+2); }' 4

echo OK
