#! /bin/bash 
 
#SBATCH --nodes=2
#SBATCH --ntasks=2
#SBATCH --output=iperf.out
#SBATCH --wait-all-nodes=1
srun iperf.sh $SLURM_JOB_NODELIST
