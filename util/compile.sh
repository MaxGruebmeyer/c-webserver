#!/bin/bash

set -eo pipefail
cflags="-Wall -Wextra -ansi -pedantic"

dir=$(dirname ${BASH_SOURCE[0]})
original=$(pwd)
out='bin'

cd "$dir/.."
mkdir -p $out

compile_cmd="gcc $cflags -o $out/server.out"
for file in src/*.c; do
    obj_file=$(echo $file | sed -r 's/\.c$/\.o/' | sed "s/src/$out/")
    gcc $cflags $file -o $obj_file -c

    compile_cmd="$compile_cmd $obj_file"
done

eval $compile_cmd

cd $original
