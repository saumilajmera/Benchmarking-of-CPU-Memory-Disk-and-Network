#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#include <time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h> 


//FILL ME IN

int bufferLen;
char *serverName;
long int GB_Data= 1000000000;  
#define PORT 8888  
char * src;
char * rec ;
char type[4];
char dataSize[5];
FILE *outfilepter;
int totThread; 
struct timeval startTime, endTime;
void errorCall(char *m)
{
    perror(m);
    exit(1);
}

typedef struct thread_info_struct{
 int sock;
 long int startIndex;
 long int endIndex;
 int ops;
 int blockLength;
}thread_info_struct;


struct sockaddr_in cli_addr;
int clilen=sizeof(cli_addr);

struct sockaddr_in ser_addr;
int serConn;
int serlen = sizeof(ser_addr);

void *TCPClientData(void *val)
{
     struct thread_info_struct *tempData= (struct thread_info_struct*)val;
	int  i=0;
   while(i<tempData->ops)
  {
      for (long int z = tempData->startIndex;z<=(tempData->endIndex);z++)
      {         
       int x = send(tempData->sock, src, bufferLen , 0);
	if(x == -1)
        {
            errorCall("sendto()");
        }          
      } 
    i++;
}

}

void *TCPServerData(void *val)
{
  
	 struct thread_info_struct *tempData= (struct thread_info_struct*)val;
	 int i=0;
	 while(i<tempData->ops)
	{
 	for (long int z = tempData->startIndex;z<=(tempData->endIndex);z++)
	{ 
	while(recv(tempData->sock, rec+(z*tempData->blockLength), tempData->blockLength, 0) >0 )
	{
	//int x = recv(tempData->sock,rec, tempData->blockLength, 0);
	//if(x==-1)
        //{	
          //  errorCall("recvfrom()");
        //} 
	
	}
	}
	i++;
	}
}

void *TCPServerPing(void *val)
{
  
	struct thread_info_struct *tempData= (struct thread_info_struct*)val;
	int i=0;
	char *rec = (char*)malloc(1);
	char *ack = (char*)malloc(1);
	memset(ack,'s',1);
	while(i<tempData->ops)
	{
		long int r=0;
	 	int x = recv(tempData->sock, rec, tempData->blockLength, 0);
		send(tempData->sock ,ack, tempData->blockLength,0);
		//printf("%s, \n", rec);
		if(x==-1)
        	{	
            	   errorCall("recvfrom()");
        	} 
		//else
		  // printf("%s \n", rec);
	   	   //r=r+x;
	  	//printf("%zu\n", x);
		
		i++;
	}
}

void *TCPClientPing(void *val)
{
	struct thread_info_struct *tempData= (struct thread_info_struct*)val;
	int  i=0;
	char *sec = (char*)malloc(1);
	char *ack = (char*)malloc(1);
	memset(sec,'A',1);
        while(i<tempData->ops)
  	{
		long int r=0;        
       		int x = send(tempData->sock, sec, bufferLen , 0);
		recv(tempData->sock,ack, bufferLen,0);
		if(x == -1)
        	{
            	  errorCall("sendto()");
        	} 
		//else
		  // printf("%s \n", ack);
			//r=r+x;
	  	//printf("%zu\n", x);          			 
		i++;
	}
//pthread_exit(0);

}
void Calculation(double exec_time)
{

double theoThroughput = 10000; //chameleon website
printf("%lf \n", exec_time);
double rate = (double)(100*GB_Data*8/exec_time);
double throughput = (double)(rate/1e6);

double theoLatency =0.0007; //chameleon  website
if(bufferLen != 1)
{
 
printf("Throughput (MB/sec) and time(secs) for %s transmission, %d concurrency, %s blocks of data for 100 GB total data are %lf and %lf and theoretical throughput value is %lf \n",type, totThread, dataSize, throughput, exec_time, theoThroughput);
    
    if(outfilepter == NULL)
        printf("error");
    else
    	fprintf(outfilepter,"%s \t %d \t %s \t %lf \t %lf \t %lf \n", type, totThread, dataSize, throughput,theoThroughput, (throughput/theoThroughput)*100);
    fclose(outfilepter);

}
else
{
    	double latency = ((exec_time)*1000.0)/1e6; 	
    	char cwd[1024];
    	if (getcwd(cwd, sizeof(cwd)) == NULL)
       		perror("getcwd() error");
	
    	strcat(cwd,"/output/network-latency.out.dat");
     	FILE *outfilepter1 = fopen(cwd, "a");
	fprintf(outfilepter1,"%s \t %d \t %s \t %lf \t %lf \t %lf\n", type, totThread, dataSize, latency, theoLatency, ((theoLatency-latency)/theoLatency)*100);
    	fclose(outfilepter1);
    printf("Latency in ms for %s communication, %d concurrency , %s data are %lf while theoretical values is %lf \n", type,totThread,dataSize, latency,theoLatency );
}
}


void TCPClient()
{

struct hostent *server_data = gethostbyname(serverName);
	struct in_addr **address = (struct in_addr **)server_data->h_addr_list;
	char ip[16];

	for(int i=0; address[i]!=NULL;i++){
		
	strcpy(ip,inet_ntoa(*address[i]));
		break;
		}

pthread_t mythread[totThread];
thread_info_struct main_thread_info[totThread];

   int totBlocks = GB_Data/bufferLen; //blocks
   int blocksPerThread = totBlocks/totThread; //blocks per thread.
 
    memset((char *) &cli_addr, 0, sizeof(cli_addr));
    cli_addr.sin_family = AF_INET;
    cli_addr.sin_port = htons(PORT);
   // cli_addr.sin_addr.s_addr= inet_addr(ip);
  if (inet_aton(ip , &cli_addr.sin_addr) == 0) 
    {
   	  fprintf(stderr, "inet_aton() failed\n");
   	  errorCall("socket creation failed");
    }

gettimeofday(&startTime, NULL);
   for(int j=0; j<totThread; j++)
  { 
   int cliConn;
  if ( (cliConn=socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        errorCall("socket");
    }

   
 	if (connect(cliConn, (struct sockaddr *)&cli_addr, sizeof(cli_addr)) < 0)
    	{
		errorCall("socket connect");
    	}
	if(bufferLen != 1)
	{
        	main_thread_info[j].startIndex = j*blocksPerThread;
		main_thread_info[j].endIndex =  ((j+1)*(blocksPerThread)) - 1;
		main_thread_info[j].blockLength = bufferLen;
		main_thread_info[j].sock = cliConn;
		main_thread_info[j].ops = 100;
		pthread_create(&mythread[j],NULL,TCPClientData,&main_thread_info[j]);
//		pthread_join(mythread[j],NULL);
	}
	else
	{
		main_thread_info[j].startIndex = 0;
		main_thread_info[j].endIndex =  0;
		main_thread_info[j].blockLength = bufferLen;
		main_thread_info[j].sock = cliConn;
		main_thread_info[j].ops = 1000000/totThread;
		pthread_create(&mythread[j],NULL,TCPClientPing,&main_thread_info[j]);
//		pthread_join(mythread[j],NULL);
	}
  }  
 for(int j=0; j<totThread;j++)
{	pthread_join(mythread[j],NULL);
}
gettimeofday(&endTime, NULL);
double exec_time = (endTime.tv_sec + (endTime.tv_usec /1000000.0)) - (startTime.tv_sec + (startTime.tv_usec/ 1000000.0)); //time in sec
Calculation(exec_time);
  
//free(src);
}

void TCPServer()
{
    int conn;
    pthread_t mythread[totThread];
thread_info_struct main_thread_info[totThread];

    //create a TCP socket
    if ((serConn=socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        errorCall("socket");
    }
     
  
    memset((char *) &ser_addr, 0, sizeof(ser_addr));
     
    ser_addr.sin_family = AF_INET;
    ser_addr.sin_port = htons(PORT);
    ser_addr.sin_addr.s_addr = htonl(INADDR_ANY);
     
 
    if(bind (serConn, (struct sockaddr *) &ser_addr, sizeof(ser_addr)) == -1)
    {
        errorCall("socket bind error");
    }
     
   listen (serConn,5);

    printf("%s\n","Listening");

   if(bufferLen != 1)
{
   int totBlocks = GB_Data/bufferLen; //blocks
   int blocksPerThread = totBlocks/totThread; //blocks per thread.
      for(int j=0; j<totThread; j++)
     {
	conn = accept (serConn, (struct sockaddr *) &ser_addr, (socklen_t*)&serlen);
	printf("%d \n", conn);
        main_thread_info[j].startIndex = j*blocksPerThread;
	main_thread_info[j].endIndex =  ((j+1)*(blocksPerThread)) - 1;
	main_thread_info[j].blockLength = bufferLen;
	main_thread_info[j].sock = conn;
	main_thread_info[j].ops = 100;
	pthread_create(&mythread[j],NULL,TCPServerData, (void *) &main_thread_info[j]);
 //	pthread_join(mythread[j],NULL);
     }  
     for(int j=0; j<totThread;j++)
     {	pthread_join(mythread[j],NULL);
	}
    //free(rec);
    //close(serConn);
}
else
{

 for(int j=0; j<totThread; j++)
     {
	conn = accept (serConn, (struct sockaddr *) &ser_addr, (socklen_t*)&serlen);
	printf("%d \n", conn);
        main_thread_info[j].startIndex = 0;
	main_thread_info[j].endIndex =  0;
	main_thread_info[j].blockLength = 1;
	main_thread_info[j].sock = conn;
	main_thread_info[j].ops = 1000000/totThread;
	pthread_create(&mythread[j],NULL,TCPServerPing, (void *) &main_thread_info[j]);
//	pthread_join(mythread[j],NULL);
     }  
    for(int j=0; j<totThread;j++)
  {	pthread_join(mythread[j],NULL);
  }
}

}

int main(int argc, char *argv[])
{ 	
    char buffer[5];
    FILE *file = fopen ( argv[3] , "r" );
    fscanf(file,"%s",type);
    fscanf(file,"\n",buffer);
    fscanf(file,"%d",&bufferLen);
    fscanf(file,"\n",buffer);
    fscanf(file,"%d",&totThread);
    fclose (file);
    serverName = argv[2];
    printf("%s \n", serverName);
    outfilepter =fopen(argv[4],"a");
    
    src = (char *)(malloc(GB_Data));
    rec = (char *)(malloc(GB_Data));
    memset(src, 'A', (GB_Data));
    if(bufferLen==1000)
	strcpy(dataSize, "1KB");
    else if (bufferLen == 32000)
	strcpy(dataSize, "32KB");
    else if (bufferLen == 1)
	strcpy(dataSize, "1B");

    if(strcmp(argv[1] ,"C") == 0)
        TCPClient();
    else 
	TCPServer();
    return 0;
}
