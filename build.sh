#!/bin/bash

EXEFILE="dijkstra_steiner"
INSTANCE="instances/instance7.txt"

SRCS=$(find 'src' -type f -name "*.c")

debug() {
    gcc -o "$EXEFILE" -Wall -Wextra -Wpedantic -Werror $SRCS
}

release() {
    gcc -o "$EXEFILE" -Wall -Wextra -Wpedantic -Werror -O3 $SRCS
}

run() {
    [ -z "$1" ] || INSTANCE="$1"
    "$EXEFILE" "$INSTANCE"
}

test() {
    gcc -o test.out "src/fibheap.c" "tests/fibheap.c" && ./test.out
}

case "$1" in
    run)      debug && run "$2" ;;
    release)  release ;;
    test)     test ;;
    *)        debug ;;
esac;

