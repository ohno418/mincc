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

assert "int main() { return 42; }" 42
assert "int main() { return 123; }" 123
assert "int main() { return 3+39; }" 42
assert "int main() { return 8+6; }" 14
assert "int main() { return 3  +39;  }" 42
assert "int main() { return   8 +  6; }" 14
assert "int main() { return 3+39+2; }" 44
assert "int main() { return 6-3; }" 3
assert "int main() { return 3+39-2; }" 40
assert "int main() { return 3*2; }" 6
assert "int main() { return 3*2+1; }" 7
assert "int main() { return 1+3*2; }" 7
assert "int main() { return 3/2+1; }" 2
assert "int main() { return 1+3/2; }" 2
assert "int main() { return 1+3/2*4; }" 5
assert "int main() { 3; return 42; }" 42
assert "int main() { 1; 2; return 3; }" 3
assert "int main() { int a=42; return a; }" 42
assert "int main() { int a=42; int b=23; return a; }" 42
assert "int main() { int a=42; int b=23; return b; }" 23
assert "int main() { int a=42; a=23; return a; }" 23
assert "int main() { int ans=42; return ans; }" 42
assert "int main() { int ans=42; int xxx=55; int yyy=123; return xxx; }" 55
assert "int main() { int ans=42; int aaa=55; int abc=123; return ans; }" 42
assert "int main() { int ans=42; int aaa=55; int abc=123; return aaa; }" 55
assert "int main() { int ans=42; int aaa=55; int abc=123; return abc; }" 123
assert "int main() { return 23>65; }" 0
assert "int main() { return 23<65; }" 1
assert "int main() { int a=23>65; return a; }" 0
assert "int main() { int a=23<65; return a; }" 1
assert "int main() { return 23<=65; }" 1
assert "int main() { return 65<=65; }" 1
assert "int main() { return 65<=23; }" 0
assert "int main() { return 23>=65; }" 0
assert "int main() { return 65>=65; }" 1
assert "int main() { return 65>=23; }" 1
assert "int main() { int a=65>=23; return a; }" 1
assert "int main() { int a=1; return a+2; }" 3
assert "int main() { int a=1; a+=1; return a; }" 2
assert "int main() { int a=1; a+=4; return a; }" 5
assert "int main() { int a=4; a-=2; return a; }" 2
assert "int main() { return 1; 2; 3; }" 1
assert "int main() { 1; return 2; 3; }" 2
assert "int main() { 1; 2; return 3; }" 3
assert "int main() { return 1; 2; return 3; }" 1
assert "int main() { int a999=123; return a999; }" 123
assert "int ret42() { return 42; } int main() { return ret42(); }" 42
assert "int add_two_num(int x, int y) { return x+y; } int main() { return add_two_num(12, 23); }" 35
assert "int add_two_num(int x, int y) { return x+y; } int main() { return add_two_num(12+2, 23); }" 37
assert "int main() { int a=3; a++; return a; }" 4
assert "int main() { int a=3; a--; return a; }" 2
assert "int main() { { return 42; } }" 42
assert "int main() { int a=4; { a++; } return a; }" 5
assert "int main() { if (0) { return 12; } return 23; }" 23
assert "int main() { if (1) { return 12; } return 23; }" 12
assert "int main() { if (5) { return 12; } return 23; }" 12
assert "int main() { if (0) return 12; return 23; }" 23
assert "int main() { if (1) return 12; return 23; }" 12
assert "int main() { if (5) return 12; return 23; }" 12
assert "int main() { if (0) { return 12; } else { return 23; } return 34; }" 23
assert "int main() { if (1) { return 12; } else { return 23; } return 34; }" 12
assert "int main() { if (5) { return 12; } else { return 23; } return 34; }" 12
assert "int main() { if (0) return 12; else return 23; return 34; }" 23
assert "int main() { if (1) return 12; else return 23; return 34; }" 12
assert "int main() { if (5) return 12; else return 23; return 34; }" 12
assert "int main() { int a=0; for (; a<5;) a++; return a; }" 5
assert "int main() { int a=0; for (a=3; a<5;) return a; return a; }" 3
assert "int main() { int a=0; int b=3; for (; a<5; a++) b=4; return a; }" 5
assert "int main() { int a=0; int b=3; for (; a<5; a++) b=4; return b; }" 4
assert "int main() { int j=5; for (int i=0; i<10; i++) j++; return i; }" 10
assert "int main() { int j=5; for (int i=0; i<10; i++) j++; return j; }" 15
assert "int main() { int a=42; return sizeof(a); }" 4
assert "int main() { int a=42; int *p=&a; return sizeof(p); }" 8
assert "int main() { return sizeof(int); }" 4
assert "int main() { return sizeof(int*); }" 8
assert "int main() { int a=42; int b=12; int *p=&a; return *p; }" 42
assert "int main() { int a=42; int b=12; int *p=&a; p++; return *p; }" 12
assert "int main() { int a=42; int b=12; int *p=&b; p--; return *p; }" 42
assert "int main() { int a=42; int b=12; int *p=&a; p=p+1; return *p; }" 12
assert "int main() { int a=42; int b=12; int *p=&b; p=p-1; return *p; }" 42
assert "int main() { int a=42; int b=12; int c=22; int *p=&a; p+=2; return *p; }" 22
assert "int main() { int a=42; int b=12; int c=22; int *p=&c; p-=2; return *p; }" 42
assert "int main() { int a=42; int *p=&a; *p = 12; return a; }" 12
assert "int main() { switch (0) { case 0: return 12; case 1: return 23; case 2: return 34; } }" 12
assert "int main() { switch (1) { case 0: return 12; case 1: return 23; case 2: return 34; } }" 23
assert "int main() { switch (2) { case 0: return 12; case 1: return 23; case 2: return 34; } }" 34
assert "int main() { switch (1) { case 0: return 12; case 1: { int ans=42; return ans; } case 2: return 34; } }" 42

echo OK
exit 0
