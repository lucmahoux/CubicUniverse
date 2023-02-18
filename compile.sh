#!/bin/sh

DIR="buildrelease"
TESTDIR="buildtest"

if [ "$#" -eq 0 ]; then 
    if [ ! -d "$DIR" ]; then
        meson setup "$DIR"
    fi
    ninja -C "$DIR"
    cd $DIR
    ./CubicUniverse
    exit
fi

if [ ! -d "$TESTDIR" ]; then
    meson setup --buildtype=debug "$TESTDIR"
fi

meson configure "$TESTDIR" -Denable_tests=true
meson configure "$TESTDIR" -Db_sanitize=address

ninja -C "$TESTDIR" test
