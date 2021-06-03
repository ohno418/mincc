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

assert "main() { 42; }" 42
assert "main() { 123; }" 123
assert "main() { 3+39; }" 42
assert "main() { 8+6; }" 14
assert "main() { 3  +39;  }" 42
assert "main() {   8 +  6; }" 14
assert "main() { 3+39+2; }" 44
assert "main() { 6-3; }" 3
assert "main() { 3+39-2; }" 40
assert "main() { 3*2; }" 6
assert "main() { 3*2+1; }" 7
assert "main() { 1+3*2; }" 7
assert "main() { 3/2+1; }" 2
assert "main() { 1+3/2; }" 2
assert "main() { 1+3/2*4; }" 5
assert "main() { 3; 42; }" 42
assert "main() { 1; 2; 3; }" 3
assert "main() { a=42; a; }" 42
assert "main() { a=42; b=23; a; }" 42
assert "main() { a=42; b=23; b; }" 23
assert "main() { ans=42; ans; }" 42
assert "main() { ans=42; xx=55; yyy=123; xx; }" 55
assert "main() { ans=42; aaa=55; abc=123; ans; }" 42
assert "main() { ans=42; aaa=55; abc=123; aaa; }" 55
assert "main() { ans=42; aaa=55; abc=123; abc; }" 123
assert "main() { 23>65; }" 0
assert "main() { 23<65; }" 1
assert "main() { a=23>65; a; }" 0
assert "main() { a=23<65; a; }" 1
assert "main() { 23<=65; }" 1
assert "main() { 65<=65; }" 1
assert "main() { 65<=23; }" 0
assert "main() { 23>=65; }" 0
assert "main() { 65>=65; }" 1
assert "main() { 65>=23; }" 1
assert "main() { a=65>=23; a; }" 1

# assert "ret_two() { 2; } main() { ret_two(); }" 2

echo OK
exit 0
