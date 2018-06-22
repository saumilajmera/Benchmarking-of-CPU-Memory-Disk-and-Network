#! /bin/bash

for k in 46 47 48 49 50 51 52; do
sbatch disk$k.slurm
done
