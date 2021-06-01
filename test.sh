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

assert "42;" 42
assert "123;" 123
assert "3+39;" 42
assert "8+6;" 14
assert "3  +39; " 42
assert "  8 +  6;" 14
assert "3+39+2;" 44
assert "6-3;" 3
assert "3+39-2;" 40
assert "3*2;" 6
assert "3*2+1;" 7
assert "1+3*2;" 7
assert "3/2+1;" 2
assert "1+3/2;" 2
assert "1+3/2*4;" 5
assert "3; 42;" 42
assert "1; 2; 3;" 3

echo OK
exit 0
