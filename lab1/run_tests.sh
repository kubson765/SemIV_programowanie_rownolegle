#!/bin/bash

THREADS=(1 2 4 8 16)
SIZES=(512 1024 2048)
RUNS=3

OUTPUT=results_laptop_podlaczony.csv
echo "version,compiler,N,threads,time_ms" > $OUTPUT

# ===== funkcja licząca średnią =====
run_avg() {
    local cmd=$1
    local N=$2
    local T=$3

    SUM=0

    for i in $(seq 1 $RUNS); do
        val=$($cmd $N $T | grep -oP 'Time: \K[0-9]+')
        SUM=$((SUM + val))
    done

    echo $((SUM / RUNS))
}

# ===== STATYCZNE =====
run_static() {
    local exe=$1
    local N=$2
    local T=$3

    val=$($exe $T | grep -oP 'Time: \K[0-9]+')

    echo "static,gpp,$N,$T,$val" >> $OUTPUT
    echo "static N=$N T=$T -> $val ms"
}

# ===== dynamic/cache =====
run_standard() {
    local name=$1
    local exe=$2
    local N=$3
    local T=$4

    val=$(run_avg "$exe" $N $T)

    echo "$name,gpp,$N,$T,$val" >> $OUTPUT
    echo "$name N=$N T=$T -> avg $val ms"
}

# ===== SIMD (różne kompilatory) =====
run_simd() {
    local compiler=$1
    local exe=$2
    local N=$3
    local T=$4

    val=$(run_avg "$exe" $N $T)

    echo "vect,$compiler,$N,$T,$val" >> $OUTPUT
    echo "vect-$compiler N=$N T=$T -> avg $val ms"
}

# ===== MAIN LOOP =====

for N in "${SIZES[@]}"; do
    for T in "${THREADS[@]}"; do

        # static
        run_static "./static_${N}.exe" $N $T

        # dynamic/cache
        run_standard "dynamic" "./dynamic.exe" $N $T
        run_standard "cache" "./cache.exe" $N $T

        # SIMD kompilatory
        run_simd "g++" "./vect_gpp.exe" $N $T
        run_simd "clang" "./vect_clang.exe" $N $T

        sleep 1
    done
done

echo "DONE -> $OUTPUT"