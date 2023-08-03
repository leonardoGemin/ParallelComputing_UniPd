#!/bin/bash

sum=0

mpicc -o mmult_transposed -O3 mmult_transposed.c
for ((i = 0; i < 25; i++)) do
    cmd="$(mpirun -np 5 ./mmult_transposed 256 2>&1)"
    #echo "${cmd}"
    sum=$(echo "$sum + $cmd" | bc -l)
done

avg=$(echo "$sum / 25" | bc -l)
echo "${avg}"
