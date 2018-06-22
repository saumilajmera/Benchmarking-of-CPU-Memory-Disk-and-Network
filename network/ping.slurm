#! /bin/bash 
 
#SBATCH --nodes=2
#SBATCH --ntasks=2
#SBATCH --output=ping.out
#SBATCH --wait-all-nodes=1
srun ping.sh $SLURM_JOB_NODELIST
