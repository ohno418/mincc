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

assert 'return 42;' 42
assert 'return 123;' 123

assert 'return 1+2;' 3
assert 'return 34+8;' 42

assert 'return 1 +2;' 3
assert 'return   1+ 2 ;' 3

assert 'return 1+2+3;' 6

assert 'return 6-2;' 4
assert 'return 6+2-3;' 5

assert 'return 6*2;' 12
assert 'return 6*2-2;' 10
assert 'return 2+6*2;' 14
assert 'return 2+6*2-3*2;' 8

assert 'return 6/2;' 3
assert 'return 6/2-1;' 2
assert 'return 1+6/2;' 4
assert 'return 6/2*3;' 9
assert 'return 2*6/2;' 6

assert 'return 3==4;' 0
assert 'return 3==3;' 1

assert 'return 3!=4;' 1
assert 'return 3!=3;' 0

assert '1; 2; return 3;' 3

assert 'a=42; return a;' 42
assert 'a=42; return a+3;' 45
assert 'a=42; z=2; return a+z;' 44

assert 'var=34; return var;' 34
assert 'foo=12; var=34; return foo+var;' 46
assert 'foo=12; faa=34; return foo+faa;' 46

assert '1; return 2; return 3;' 2

assert '{ return 42; }' 42
assert '{ foo=12; return foo; }' 12
assert '{ foo=12; faa=34; return foo+faa; }' 46
assert '{ 1; return 2; return 3; }' 2

assert ';; return 5;' 5

assert 'if (1) return 3; return 4;' 3
assert 'if (0) return 3; return 4;' 4
assert 'if (1) { return 3; } return 4;' 3
assert 'if (0) { return 3; } return 4;' 4
assert 'if (1-1) return 3; return 4;' 4
assert 'if (3-2) return 3; return 4;' 3
assert 'if (1) { if (1) return 42; return 12; } return 2;' 42
assert 'if (1) { if (0) return 42; return 12; } return 2;' 12
assert 'if (0) { if (1) return 42; return 12; } return 2;' 2
assert 'if (1) { if (1) return 42; return 12; } if (0) return 123; return 2;' 42
assert 'if (0) { if (1) return 42; return 12; } if (1) return 123; return 2;' 123
assert 'if (0) { if (1) return 42; return 12; } if (0) return 123; return 2;' 2

echo OK
