#! /bin/bash

for k in 37 38 39 40 41 42 43 44 45; do 
sbatch disk$k.slurm
done
