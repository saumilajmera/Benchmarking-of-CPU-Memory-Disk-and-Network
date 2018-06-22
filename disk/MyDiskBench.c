#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <math.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

//FILL ME IN

long long FILE_CAP=10000000000LL;
long int totBlocks;
long int workPerThread;
int totThread;
int workload;
char type[4];
FILE *outfilepter;
char dataSize[7];


//structure to hold each thread data
typedef struct thread_info_struct{
 long int startIndex;
 long int endIndex;
 long int blockLength;
}thread_info_struct;



void* randRead_access(void* threadData)
{
  srand(time(NULL));

   int ops=0;

   int random;
   struct thread_info_struct *tempData= (struct thread_info_struct*)threadData;
   char* tempBuffer = malloc(tempData->blockLength *  sizeof(char)); 
   long int start = tempData->startIndex;
   long int count =0; 
   FILE *fptr;
   fptr = fopen("/tmp/fileR.txt", "r+");
   if(fptr == NULL)
	printf("File not found");
   size_t ret ;
   int i;
   //system("echo 3 | sudo tee /proc/sys/vm/drop_caches");
   long int end= tempData->endIndex;
if(tempData->blockLength == 1000)
{ 
   while(count < (1000000/totThread))
  {
	
            random = (rand()%(workPerThread))+start;
            fseek(fptr,random,SEEK_CUR);
	    ret = fread(tempBuffer,sizeof(char),tempData->blockLength, fptr);
	    //printf("%zu\n", ret);
	    count++;
   }
	  // printf("%ld\n", count);
}
else
{
fseek(fptr,tempData->startIndex,SEEK_CUR);
      for(i=tempData->startIndex;i<=tempData->endIndex;i++){
            random = (rand()%(end-start))+start;
            fseek(fptr,random,SEEK_CUR);
	    //memset(tempBuffer, '\0', tempData->blockLength);
	    ret = fread(tempBuffer,sizeof(char),tempData->blockLength, fptr);
	}
}
    fclose(fptr);
    free(tempBuffer);
}

void* randWrite_access(void* threadData)
{

   srand(time(NULL));

   int ops=0;

   int random;
   struct thread_info_struct *tempData= (struct thread_info_struct*)threadData;
   char* tempBuffer = malloc(tempData->blockLength *  sizeof(char)); //common read write buffer
   memset(tempBuffer, 'a',tempData->blockLength);
   long int start = tempData->startIndex;
   long int count =0; 
   FILE *fptr;
   fptr = fopen("/tmp/FileW.txt", "a+");
   size_t ret;
   int i;
   long int end= tempData->endIndex;
if(tempData->blockLength == 1000)
{ 
   while(count < (1000000/totThread))
  {
         random = (rand()%(workPerThread))+start;
	 fseek(fptr,random,SEEK_CUR);
	 ret = fwrite(tempBuffer,sizeof(char),tempData->blockLength, fptr);
	 count++;
  }
}
else
{
fseek(fptr,tempData->startIndex,SEEK_SET);
     for(i=tempData->startIndex;i<=tempData->endIndex;i++){
           random = (rand()%(end-start))+start;
	   fseek(fptr,random,SEEK_SET);
	    ret = fwrite(tempBuffer,sizeof(char),tempData->blockLength, fptr);
	count++;
	}
}
  fclose(fptr);
 free(tempBuffer);
}


void* seqWrite_access(void* threadData)
{	

   int ops=0;

   int random;
   struct thread_info_struct *tempData= (struct thread_info_struct*)threadData;
   char* tempBuffer = malloc(tempData->blockLength *  sizeof(char)); //common read write buffer
   memset(tempBuffer, 'a',tempData->blockLength);
   long int start = tempData->startIndex;
   long int count =0; 
   FILE *fptr;
   fptr = fopen("/tmp/FileW.txt", "a+");
   size_t ret ;
   int i = 0;
   long int end= tempData->endIndex;
      fseek(fptr,start,SEEK_CUR);
      for(i=tempData->startIndex;i<=tempData->endIndex;i++)
	{
	    ret = fwrite(tempBuffer,sizeof(char),tempData->blockLength, fptr);
	//printf("%zu\n", ret);
	}
  fclose(fptr);
 free(tempBuffer);
}

void* sampleWrite(void* threadData)
{	

   int ops=0;

   int random;
   struct thread_info_struct *tempData= (struct thread_info_struct*)threadData;
   char* tempBuffer = malloc(tempData->blockLength *  sizeof(char)); //common read write buffer
   memset(tempBuffer, 'a',tempData->blockLength);
   long int start = tempData->startIndex;
   long int count =0; 
   FILE *fptr;
   fptr = fopen("/tmp/fileR.txt", "a+");
   size_t ret ;
   int i = 0;
   long int end= tempData->endIndex;
      for(i=tempData->startIndex;i<=tempData->endIndex;i++)
	{
	    ret = fwrite(tempBuffer,1,tempData->blockLength, fptr);
	}
  fclose(fptr);
 free(tempBuffer);
}

void* seqRead_access(void* threadData)
{
   long int i;
   struct thread_info_struct *tempData= (struct thread_info_struct*)threadData;
   char* tempBuffer = malloc(tempData->blockLength *  sizeof(char)); //common read write buffer
   long int start= tempData->startIndex;
   long int count =0;
   long int end= tempData->endIndex;

 
   FILE *fptr;

   fptr = fopen("/tmp/fileR.txt", "r");
   if(fptr == NULL)
	printf("File not found");
   size_t ret ;
 fseek(fptr,start,SEEK_SET);
      for(i=tempData->startIndex;i<=tempData->endIndex;i++)
	{
	//memset(tempBuffer, '\0', tempData->blockLength);
        ret = fread(tempBuffer,sizeof(char),tempData->blockLength, fptr);
	 //printf("%zu\n", ret);
	}
    
    fclose(fptr);
   free(tempBuffer);
}

void ExecuteDisk()
{

  pthread_t mythread[totThread];
  thread_info_struct main_thread_info[totThread];
  long int convtWorkload;
  convtWorkload = workload*1000;
  int i;

  if(convtWorkload==1000)
  {
	strcpy(dataSize, "1KB");
	FILE_CAP = 1000000000;//1GB
  }
  else if (convtWorkload == 1000000)
	strcpy(dataSize, "1MB");
  else if (convtWorkload == 10000000)
	strcpy(dataSize, "10MB");
  else if (convtWorkload == 100000000)
	strcpy(dataSize, "100MB");

  struct timeval startTime, endTime;


  totBlocks = FILE_CAP/convtWorkload;
  workPerThread = totBlocks/totThread;
  double theoThroughput; 
  double theoIOPS;
  double theoLatency = 0.5/totThread; //0.5 msec given in specification of disk
 
 
 if(strcmp(type, "RR")==0 || strcmp(type, "RS")==0)
 {
   theoThroughput = strcmp(type, "RR")==0 ? 372.0*totThread:540.0*totThread; //specification of disk
    long int Blocks = convtWorkload==1000 ? FILE_CAP/10000000 : FILE_CAP/100000000;
    long int tempWork = Blocks/10;
    pthread_t tempThread[10];
    thread_info_struct tempThread_info[10];

   for(i=0;i<10;i++){
        tempThread_info[i].startIndex = i*tempWork;
	tempThread_info[i].endIndex =  ((i+1)*(tempWork)) - 1;
	tempThread_info[i].blockLength = convtWorkload==1000 ? 10000000 : 100000000;
	pthread_create(&tempThread[i], NULL, (void *) sampleWrite, (void *) &tempThread_info[i]);
  	//printf("%ld \n", tempThread_info[i].startIndex);
        //printf("%ld \n", tempThread_info[i].endIndex);
        //printf("%ld \n", tempThread_info[i].blockLength);

}

for(i=0;i<10;i++){
	pthread_join(tempThread[i], NULL);
}
}
else
{
  theoThroughput = strcmp(type, "WR")==0 ? 172.0*totThread : 410.0*totThread; //specification of disk
}


  gettimeofday(&startTime, NULL);
  for(i=0;i<totThread;i++){
	main_thread_info[i].startIndex = i*workPerThread;
	main_thread_info[i].endIndex =  ((i+1)*(workPerThread)) - 1;
	main_thread_info[i].blockLength = convtWorkload;

       if(strcmp(type, "RR")==0 && convtWorkload != 1000)
            pthread_create(&mythread[i], NULL, (void *) randRead_access, (void *) &main_thread_info[i]);
       else if(strcmp(type, "RS")==0 && convtWorkload != 1000)
            pthread_create(&mythread[i], NULL, (void *) seqRead_access, (void *) &main_thread_info[i]);	
       else if(strcmp(type, "WS")==0 && convtWorkload != 1000)
            pthread_create(&mythread[i], NULL, (void *) seqWrite_access, (void *) &main_thread_info[i]);
       else if(strcmp(type, "WR")==0 && convtWorkload != 1000)
            pthread_create(&mythread[i], NULL, (void *) randWrite_access, (void *) &main_thread_info[i]);
       else if(strcmp(type, "RR")==0 && convtWorkload == 1000)
            pthread_create(&mythread[i], NULL, (void *) randRead_access, (void *) &main_thread_info[i]);
       else if(strcmp(type, "WR")==0 && convtWorkload == 1000)
           pthread_create(&mythread[i], NULL, (void *) randWrite_access, (void *) &main_thread_info[i]);

 //	printf("%ld \n", main_thread_info[i].startIndex);
  //      printf("%ld \n", main_thread_info[i].endIndex);
//        printf("%ld \n", main_thread_info[i].blockLength);
    }

	for (i = 0; i < totThread; i++) {
        pthread_join(mythread[i], NULL);
	}
  gettimeofday(&endTime, NULL);
  double exec_time = (endTime.tv_sec + (endTime.tv_usec /1000000.0)) - (startTime.tv_sec + (startTime.tv_usec/ 1000000.0)); //time in sec

double throughput = (double)(FILE_CAP/exec_time)/(1000*1000);

if(convtWorkload != 1000)
{
 
printf("Throughput in MB/sec and Time in sec for %s pattern, for %d concurrency, for %s blocks of data for 10 GB total data are %lf and %lf and theoretical throughput value and efficiency are %lf and %lf \n",type, totThread, dataSize, throughput, exec_time, theoThroughput, (throughput/theoThroughput)*100);
    
    if(outfilepter == NULL)
        printf("error");
    else
    	fprintf(outfilepter,"%s \t %d \t %s \t %lf \t %lf \t %lf \n", type, totThread, dataSize, throughput,theoThroughput, (throughput/theoThroughput)*100);
    fclose(outfilepter);

}
else
{
    	double latency = (exec_time*1000.0)/1e6; 	
    	double IOPS = (double)((1e6/exec_time));
	theoIOPS = (double)(theoThroughput*1000000/1000);

    	char cwd[1024];
    	if (getcwd(cwd, sizeof(cwd)) == NULL)
       		perror("getcwd() error");
	
    	strcat(cwd,"/output/disk-latency-IOPS.out.dat");
     	FILE *outfilepter1 = fopen(cwd, "a");
	fprintf(outfilepter1,"%s \t %d \t %s \t %lf \t %lf \t %lf \t %lf \t %lf \t %lf\n", type, totThread, dataSize, latency, theoLatency, ((theoLatency-latency)/theoLatency)*100, IOPS, theoIOPS, (IOPS/theoIOPS)*100);
    	fclose(outfilepter1);
    printf("Latency in ms and IOPS for %s pattern, %d concurrency , %s data are %lf and %lf while theoretical values are %lf and %lf \n", type,   totThread, dataSize, latency,IOPS,theoLatency, theoIOPS );
}

}

int main(int argc, char *argv[]) {

    char buffer[5];   
    FILE *file = fopen ( argv[1] , "r" );
    outfilepter=fopen(argv[2], "a");
    fscanf(file,"%s",type);
    fscanf(file,"\n",buffer);
    fscanf(file,"%d",&workload);
    fscanf(file,"\n",buffer);
    fscanf(file,"%d",&totThread);
    fclose (file);
    ExecuteDisk();

    return 0;
}


