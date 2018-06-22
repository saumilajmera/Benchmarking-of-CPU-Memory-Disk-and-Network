#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <math.h>
#include <sys/time.h>
#include <time.h>

//FILL ME IN

long int GB_Data=1000000000;//Data Length
long int totBlocks;
long int blocksPerThread;
int totThread;
int workload;
char type[4];//operation type
char dataSize[5]; //data size
FILE *outfilepter;

//structure to hold each thread data
typedef struct thread_info_struct{
 long int startIndex;
 long int endIndex;
 long int no_of_ops;
 long int blockLength;
}thread_info_struct;


char *src; //source string pointer
char *dest; // destination string pointer

void* rand_access(void* threadData)
{
	   srand(time(NULL));
	   int ops=0;
	 
	   long random;
	   struct thread_info_struct *tempData= (struct thread_info_struct*)threadData;
	   long int i = tempData->startIndex;
	   long int end= tempData->endIndex;
	   while(ops<tempData->no_of_ops)
	   {
	    i= tempData->startIndex;
	    for(i=tempData->startIndex;i<tempData->endIndex;i++){
		 random = (rand()%(end-i))+i;
		 memcpy(dest+(random*tempData->blockLength),src+(random*tempData->blockLength),tempData->blockLength);
		}
		ops++;
	    }
}


void* latrand_access(void* threadData)
{
	   srand(time(NULL));
	   int ops=0;
	 
	   long random;
	   struct thread_info_struct *tempData= (struct thread_info_struct*)threadData;
	   long int i = tempData->startIndex;
	   long int end= tempData->endIndex;
	   while(ops<tempData->no_of_ops)
	   {
	     random = (rand()%(end-i))+i;
	      memcpy(dest+random,src+random,tempData->blockLength);
	      ops++;
	    }
}

void* latseq_access(void* threadData)
{
 
	   int ops=0;
	   struct thread_info_struct *tempData= (struct thread_info_struct*)threadData;
	   long int i= tempData->startIndex;
	   while(ops<tempData->no_of_ops)
	   {
	      i++;
	      memcpy(dest+i,src+i,tempData->blockLength);
	      ops++;
	   }
}

void* seq_access(void* threadData)
{
 
	   int ops=0;
	   struct thread_info_struct *tempData= (struct thread_info_struct*)threadData;
	   long int i= tempData->startIndex;
	   long int end= tempData->endIndex;
	  
	   while(ops<tempData->no_of_ops)
	   {
	      i= tempData->startIndex;
	     for(i=tempData->startIndex;i<tempData->endIndex;i++)
		{
		    memcpy(dest+(i* tempData->blockLength),src+(i* tempData->blockLength),tempData->blockLength);
	     	}
		ops++;
	    }
}

void ExecuteRam()
{
	  pthread_t mythread[totThread];
	  thread_info_struct main_thread_info[totThread];
	  int i;

	  struct timeval startTime, endTime;

	  totBlocks = GB_Data/workload; //blocks
	  blocksPerThread = totBlocks/totThread; //blocks per thread.
	  //printf("%ld \n",totBlocks);
	  //printf("%ld \n",blocksPerThread);

	  if(workload==1)
		strcpy(dataSize, "1B");
	  else if (workload == 1000)
		strcpy(dataSize, "1KB");
	  else if (workload == 1000000)
		strcpy(dataSize, "1MB");
	  else if (workload == 10000000)
		strcpy(dataSize, "10MB");
	  src = (char *)(malloc(GB_Data));
	  dest = (char *)(malloc(GB_Data));
	  memset(src,'S',GB_Data);


	  gettimeofday(&startTime, NULL);

	  for(i=0;i<totThread;i++){
		main_thread_info[i].startIndex = i*blocksPerThread;
		main_thread_info[i].endIndex =  ((i+1)*(blocksPerThread)) - 1;
		main_thread_info[i].blockLength = workload;

	 	if(strcmp(type, "RWR")==0 && workload != 1)
		{	 
		   main_thread_info[i].no_of_ops = 100;
		   pthread_create(&mythread[i], NULL, (void *) rand_access, (void *) &main_thread_info[i]);
		}
		else if(strcmp(type, "RWS")==0 && workload != 1)
		{
		    main_thread_info[i].no_of_ops = 100;
		    pthread_create(&mythread[i], NULL, (void *) seq_access, (void *) &main_thread_info[i]);
		}
		else if(strcmp(type, "RWS")==0 && workload == 1)
		{
		    main_thread_info[i].no_of_ops = 100000000;
		    pthread_create(&mythread[i], NULL, (void *) latseq_access, (void *) &main_thread_info[i]);
		}
		else if(strcmp(type, "RWR")==0 && workload == 1)
		{
		   main_thread_info[i].no_of_ops = 100000000;
		   pthread_create(&mythread[i], NULL, (void *) latrand_access, (void *) &main_thread_info[i]);
		}
	    }

		for (i = 0; i < totThread; i++) {
		  pthread_join(mythread[i], NULL);
		}
	  	gettimeofday(&endTime, NULL);
	  	double RAM = (double)((2133*2*64*2)/8);
	 	double theoretical = (double)(RAM/1e3);
	  
	  	double exec_time = (endTime.tv_sec + (endTime.tv_usec /1000000.0)) - (startTime.tv_sec + (startTime.tv_usec/ 1000000.0)); //time in sec
	  	double latency = (exec_time*1000000.0)/1e8;
		double theoLatency = 0.0141;
	  	double throughput = (double)((GB_Data*100)/exec_time)/1e9;
		//condition for block size other than 1B
	       if(workload !=1)
	       {
	  	 printf("Throughput in GB/ps for %s pattern, for %d concurrency, for %s data is %lf  \n",type, totThread, dataSize, throughput);
	    
	    	if(outfilepter == NULL)
			printf("error");
	    	else
	    		fprintf(outfilepter,"%s \t %d \t %s \t %lf \t %lf \t %lf \n", type, totThread, dataSize, throughput, theoretical, (throughput/theoretical)*100);
			//fprintf(outfilepter, "SA");
	    	fclose(outfilepter);
		}
		//1B block size
		else
		{
			 char cwd[1024];
	   	         if (getcwd(cwd, sizeof(cwd)) == NULL)
	       			perror("getcwd() error");
	
	       		 strcat(cwd,"/output/memory-latency.out.dat");
			 FILE *outfilepter1 = fopen(cwd, "a");
			 fprintf(outfilepter1,"%s \t %d \t %s \t %lf \t %lf \t %lf \n", type, totThread, dataSize, latency, theoLatency, ((theoLatency-latency)/theoLatency)*100);
	    	fclose(outfilepter1);
			 printf("Latency for %s pattern, %d concurrency, %s data is %lf and theoretical latency is %lf and efficiency is %lf \n", type, totThread, dataSize, latency, theoLatency,((theoLatency-latency)/theoLatency)*100);
		}

	free(src);
	free(dest);

}

int main(int argc, char *argv[]) {

    char buffer[5];
    FILE *file  = fopen(argv[1], "r");
    outfilepter = fopen(argv[2], "a");
    fscanf(file,"%s",type);
    fscanf(file,"\n",buffer);
    fscanf(file,"%d",&workload);
    fscanf(file,"\n",buffer);
    fscanf(file,"%d",&totThread);
    fclose (file);
    ExecuteRam();

    return 0;
}

