###### Compile #####

make all

########### Network Benchmark ###########

<mode (TCP UDP)>
<block size (1B, 1KB, 32KB)> # in bytes 1, 1000, 32000
<number of threads (1,2,4,8)>


###### Steps to Run ######

1) Run the Makefile which will compile the MyNETBench-TCP.c and My NETBench-UDP.c code with required flags into executable MyNETBench-TCP and MyNETBench-UDP

2) Run script file create_slurms using bash create_slurms command which will generate config files for each configurations. which contains slurm commands to srun runUDP.sh or runTCP.sh by passing required input arguments as input DAT file and output DAT file.

3) Now manually one needs to submit each of file like sbatch TCP-1-1.slurm which will call runTCP.sh or (runUDP.sh) which will run same executable one for server and one for client by passing S or C for client server identification. I am unable to create run.sh files unlike previous three benchmarks.

4) Note that in runTCP.sh and runUDP.sh , I have specifically mentioned path 
//exports/home/sajmera4/network/MyNETBench-TCP, 
//exports/home/sajmera4/network/MyNETBench-UDP,
This may require changes depending upon how you are running. 

5) Output for TCP and UDP for throughput is written into file in the output folder named network-TCP-1thread.out.dat  with required set of values. Output for latency IOPS is written into file in the output folder name network-latency-.out.dat with required set of values.


