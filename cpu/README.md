## Compile

make all

########### CPU Benchmark Input ###########

<workload type (QP, HP, SP, DP)>
<number of threads (1,2,4)>


###### Steps To Run ##########

1) Run the Makefile which will compile the MyCPUBench.c code with required flags into executable MyCPUBench 

2) Run script file create_slurms using bash create_slurms command which will generate config files naming like cpu1.slurm, cpu2.slurm etc. which contains slurm commands to run executable by passing required input arguments as input DAT file and output DAT file.

3) Now I have prepared single script file per 9 config files to submit 9 jobs to different nodes. run1.sh and run2.sh are two script files. 

4) Now do sbatch run1.sh and after 9 files has been processed do sbatch run2.sh for running remaining jobs.

5) Output is written into file into output folder named cpu_SP_1thread.out.dat  with required set of values.
