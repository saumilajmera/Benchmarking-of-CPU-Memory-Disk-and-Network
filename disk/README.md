###### Compile ######

make all

########### Disk Benchmark Input ###########

<mode (RS WS RR WR)>
<block size (1KB, 1000KB, 10000KB, 100000KB)>
<number of threads (1,2,4)> # For throughput
<number of threads (1,2,4,8,16,32,64,128)> # For latency


###### Steps to Run ############


1) Run the Makefile by make all which will compile the MyDiskBench.c code with required flags into executable MyDiskBench 

2) Run script file create_slurms using bash create_slurms command which will generate config files naming like disk1.slurm, disk2.slurm etc. which contains slurm commands to run exe by passing required input arguments as input DAT file and output DAT file.

3) Now I have prepared single script file per 9 config files to submit 9 jobs to different nodes. Here there are 52 experiments so there are 9 run.sh files run1.sh, run2.sh, run3.sh, run4.sh, run5.sh, run6.sh containing all jobs.

4) Now do sbatch run1.sh and after 9 files has been processed do sbatch run2.sh for running remaining jobs. Similarly for other script files.

5) Output is written into file in the output folder named memory-RR-1-1thread.out.dat  with required set of values. Output for latency is written into file in the output folder name disk-latency-IOPS.out.dat with required set of values.
