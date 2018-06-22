##### Compile #####

make all

########### Memory Benchmark Input ###########

<benchmark_type (RWS RWR)>
<block size (1B, 1KB, 1MB, 10MB)> # in bytes
<number of threads (1,2,4)>



##### Steps To Run #############

1) Run the Makefile  by make all which will compile the MyRAMBench.c code with required flags into executable MyRAMBench
 
2) Run script file create_slurms using bash create_slurms command which will generate config files naming like mem1.slurm, mem2.slurm etc. which contains slurm commands to run exe by passing required input arguments as input DAT file and output DAT file.

3) Now I have prepared single script file per 9 config files to submit 9 jobs to different nodes. run1.sh, run2.sh and run3.sh  script files. 

4) Now do sbatch run1.sh and after 9 files has been processed do sbatch run2.sh for running remaining jobs. Similarly for other script files

5) Output is written into file in the output folder named memory-RWR-1-1thread.out.dat  with required set of values. Output for latency is written into file in the output folder name memory-latency.out.dat with required set of values.
