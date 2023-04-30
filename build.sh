#!/bin/bash

INSTANCE="instance4.txt"

SRCS=$(find 'src' -type f -name "*.c")

debug() {
    set -x
    gcc $SRCS
}

release() {
    set -x
    gcc -O3 $SRCS
}

run() {
    [ -z "$1" ] || INSTANCE="$1"
    ./a.out "$INSTANCE"
}

case "$1" in
    run)      debug && run "$2" ;;
    release)  release ;;
    *)        debug ;;
esac;

