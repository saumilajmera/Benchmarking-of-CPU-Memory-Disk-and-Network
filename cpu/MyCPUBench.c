#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>
#include <dirent.h>
#include <ctype.h>
#include <time.h>
#include <immintrin.h>

//FILL ME IN

const long long int no_operations = 1000000000000LL; // 1 trillion operations
int countThread; // threads
char threadtype[4]; // data
FILE *outfilepter;
int ips; //instructions per cycle
long long int ops; // operations per thread

void *SFlps(void *p)
{
	long long int iterator;
	ips = 32;
	
	__m256 s1 = _mm256_set_ps(4.0,8.0,6.0,2.0,11.0,12.0,22.0,26.0);
	__m256 s2 = _mm256_set_ps(71.0,89.0,1.0,66.0,55.0,51.0,16.0,13.0);
	__m256 s3 = _mm256_set_ps(71.0,89.0,69,32.0,55.0,56.0,16.0,13.0);

	//32 SP FLOPs/cycle: two 8-wide FMA (fused multiply-add) instructions in one cycle
	// loop unrolling 
	for(iterator=0;iterator< ops/32 ;iterator++){
		__m256 result1 = _mm256_fmadd_ps(s1,s2,s3);
		__m256 result2 = _mm256_fmadd_ps(s1,s3,s2);
	}
	
}

void *DFlps(void *q)
{ 	
	long long int iterator;
    
	ips=16;
	__m256d d1 = _mm256_set_pd(2.5,2.2,2.4,3.4);
	__m256d d2 = _mm256_set_pd(1.8,61.5,4.4,4.0);
	__m256d d3 = _mm256_set_pd(1.8,1.5,4.4,10.6);
	
	//16 DP FLOPs/cycle: two 4-wide FMA (fused multiply-add) instructions in one cycle
	// loop unrolling 
	for(iterator=0;iterator< ops/16 ;iterator++){
		__m256d result1 = _mm256_fmadd_pd(d1, d2, d3);
		__m256d result2 = _mm256_fmadd_pd(d1, d3, d2);
	}
	
}

void *QFlps(void *q)
{
	ips =128;
	long long int iterator;
	__m256i q1 = _mm256_set_epi8( 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a',
			'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a',
			'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a' );	
	__m256i q2 = _mm256_set_epi8( 'Z', 'Z', 'Z', 'Z', 'Z', 'Z', 'Z', 'Z', 'Z', 'Z', 'Z',
			'Z', 'Z', 'Z', 'Z', 'Z', 'Z', 'Z', 'Z', 'Z', 'Z', 'Z', 'Z', 'Z',
			'Z', 'Z', 'Z', 'Z', 'Z', 'Z', 'Z', 'Z' );

	//128 QP FLOPs/cycle: // loop unrolling 
	for(iterator=0;iterator< ops/128 ;iterator++){
		__m256i result1 = _mm256_add_epi8(q1, q2);
		__m256i result2 = _mm256_add_epi8(q1, q2);
		__m256i result3 = _mm256_add_epi8(q1, q2);
		__m256i result4 = _mm256_add_epi8(q1, q2);
	}
	
}

void *HFlps(void *q)
{
	long long int iterator;
	ips= 64;
	__m256i h1 = _mm256_set_epi16( 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 );
	__m256i h2 = _mm256_set_epi16( 78, 100, 4, 78, 69, 18, 1, 81, 66, 1, 58, 1, 55, 32, 13, 44 );

	//64 HP FLOPs/cycle: // loop unrolling 
	for(iterator=0;iterator< ops/64 ;iterator++){
		__m256i result1 = _mm256_add_epi16(h1, h2);
		__m256i result2 = _mm256_add_epi16(h1, h2);
		__m256i result3 = _mm256_add_epi16(h1, h2);
		__m256i result4 = _mm256_add_epi16(h1, h2);	
	}
}

void executeCPU()
{ 
        
	pthread_t mythread[countThread];
        ops = (long long int) no_operations/countThread; // dividing by no of threads
	//printf("%lld \n", ops);
	int i;
	double exeTime=0.0, gigaOps=0.0;
	struct timeval startTime, endTime;
	
	gettimeofday(&startTime, NULL);

	for(i = 0; i < countThread; i++)
	{	
		if(strcmp(threadtype,  "QP")==0)
		{
		  pthread_create(&mythread[i], NULL,QFlps, NULL);		 
		}
		
		else if(strcmp(threadtype,  "HP")==0)
		{
		    pthread_create(&mythread[i], NULL,HFlps, NULL);		 
		}
		
		else if(strcmp(threadtype,  "SP")==0)
		{  
		  pthread_create(&mythread[i], NULL,SFlps, NULL);
		 		
		}
		
		else if(strcmp(threadtype,  "DP")==0)
		{
		  pthread_create(&mythread[i], NULL,DFlps, NULL);		 
		}
	}
	for(i = 0; i < countThread; i++)
	{
 	pthread_join(mythread[i],NULL);		
	}
	
	gettimeofday(&endTime, NULL);
 	double exec_time = (endTime.tv_sec + (endTime.tv_usec /1000000.0)) - (startTime.tv_sec + (startTime.tv_usec/ 1000000.0)); //time in sec
	
	double theoretical = 2.3*2*ips*1; // theoretical value of Intel Xeon E5-2670 processor of Chameleon Compute Node

	gigaOps = ((double)(no_operations/exec_time))/1e9;
	
      	  printf("Number of Giga OPS/Sec for %s workload, %d concurrency is %lf , Time Taken : %lf, Theoretical value is  : %lf, Efficiency is  : %lf  \n",threadtype, countThread,gigaOps,exec_time, theoretical, (gigaOps/theoretical)*100);
    
	
    if(outfilepter == NULL)
        printf("error");
    else
    	fprintf(outfilepter,"%s \t %d \t %lf \t %lf \t %lf\n", threadtype, countThread, gigaOps,theoretical, (gigaOps/theoretical)*100);
    fclose(outfilepter);
	

}


int main(int argc, char *argv[]) {

    char buffer[5];
    printf("%lld \n", no_operations);
    FILE *file = fopen ( argv[1] , "r" );
    if(file == NULL)
    {
	printf("File not present");	
       return(0);
     }
    fscanf(file,"%s",threadtype);
    fscanf(file,"\n",buffer);
    fscanf(file,"%d",&countThread);
    fscanf(file,"\n",buffer);   
    fclose (file);
    outfilepter = fopen(argv[2], "a");
    executeCPU();
    return(0);
}



