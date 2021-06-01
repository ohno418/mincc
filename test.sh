#!/bin/bash
./mincc > tmp.s
gcc -o tmp tmp.s
./tmp

retval="$?"
if [ "$retval" == "42" ]
then
  echo OK
else
  echo FAIL
  exit 1
fi

exit 0
