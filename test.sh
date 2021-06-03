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
assert "main() { int a=42; a; }" 42
assert "main() { int a=42; int b=23; a; }" 42
assert "main() { int a=42; int b=23; b; }" 23
assert "main() { int a=42; a=23; a; }" 23
assert "main() { int ans=42; ans; }" 42
assert "main() { int ans=42; int xxx=55; int yyy=123; xxx; }" 55
assert "main() { int ans=42; int aaa=55; int abc=123; ans; }" 42
assert "main() { int ans=42; int aaa=55; int abc=123; aaa; }" 55
assert "main() { int ans=42; int aaa=55; int abc=123; abc; }" 123
assert "main() { 23>65; }" 0
assert "main() { 23<65; }" 1
assert "main() { int a=23>65; a; }" 0
assert "main() { int a=23<65; a; }" 1
assert "main() { 23<=65; }" 1
assert "main() { 65<=65; }" 1
assert "main() { 65<=23; }" 0
assert "main() { 23>=65; }" 0
assert "main() { 65>=65; }" 1
assert "main() { 65>=23; }" 1
assert "main() { int a=65>=23; a; }" 1
assert "main() { int a=1; a+2; }" 3
assert "main() { int a=1; a+=1; a; }" 2
assert "main() { int a=1; a+=4; a; }" 5
assert "main() { int a=4; a-=2; a; }" 2

echo OK
exit 0
