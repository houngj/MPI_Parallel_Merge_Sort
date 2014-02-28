#!/bin/bash
for t in 1 2 4 8; do 
    for i in 16 24 32 40 48 56 64 72 80 88 96 104; do
	echo "($t, $i)";
	mpiexec -n $t ./parallelMergesort $i;
	
    done; 
done
