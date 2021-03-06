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

assert 42 42
assert 123 123

assert '1+2' 3
assert '34+8' 42

assert '1 +2' 3
assert '  1+ 2 ' 3

assert '1+2+3' 6

echo OK
