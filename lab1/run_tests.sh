#!/bin/bash

# programy
STATIC=./static.exe
DYNAMIC=./dynamic.exe
CACHE=./cache.exe
SIMD=./vec.exe

# parametry testów
THREADS=(1 2 4 8 16)
SIZES=(1024 2048 4096)

# plik wynikowy
OUTPUT=results.csv

echo "version,N,threads,time_ms" > $OUTPUT

run_test() {
    local name=$1
    local cmd=$2
    local N=$3
    local T=$4

    SUM=0
    RUNS=3

    for i in $(seq 1 $RUNS); do
        val=$($cmd $N $T | grep -oP 'Time: \K[0-9]+')
        SUM=$((SUM + val))
    done

    AVG=$((SUM / RUNS))

    echo "$name,$N,$T,$AVG" >> $OUTPUT
    echo "$name N=$N T=$T -> avg $AVG ms"
}

for N in "${SIZES[@]}"; do
    for T in "${THREADS[@]}"; do

        run_test "dynamic" "$DYNAMIC" $N $T
        run_test "cache" "$CACHE" $N $T
        run_test "simd" "$SIMD" $N $T

        # statyczna nie ma N
        result=$($STATIC $T | grep -o '[0-9]\+')
        echo "static,$N,$T,$result" >> $OUTPUT
        echo "static N=$N T=$T -> $result ms"

    done
done

echo "DONE -> wyniki w $OUTPUT"
