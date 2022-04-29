#! /bin/bash
rm a.out 2> /dev/null

cp disk2 diskimage

# ./mkdisk

gcc *.c -w

./a.out


