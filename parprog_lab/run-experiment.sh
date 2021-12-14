#!/bin/bash

for p in $(seq 1 16); do
  for i in $(seq 1 3); do
    echo "run: ($i, $p)";
    mpirun -n $p ./$1 10 1000000 &>> $2
  done
done
