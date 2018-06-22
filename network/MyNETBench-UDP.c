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
char type[4];
#define PORT 8888  
char * src;
char * rec ;
char dataSize[5];
FILE *outfilepter;

struct timeval startTime, endTime;
int totThread; 
void errorCall(char *m)
{
    perror(m);
    exit(1);
}


typedef struct thread_info_struct{
 int sock;
 long int startIndex;
 long int endIndex;
 int blockLength;
 int ops;
}thread_info_struct;


struct sockaddr_in cli_addr;
int clilen=sizeof(cli_addr);


struct sockaddr_in ser_addr, send_addr;
int serConn;
int addlen = sizeof(ser_addr);
int serlen = sizeof(send_addr);

void *UDPClientData(void *val)
{
	long int r=0;
	int i=0;
	struct thread_info_struct *tempData= (struct thread_info_struct*)val;
	while(i<tempData->ops){
	long int r=0;
        for (long int z = tempData->startIndex;z<(tempData->endIndex);z++)
        {        
	 
       	int x = sendto(tempData->sock, src+(z*tempData->blockLength), bufferLen , 0 ,(struct sockaddr *) &cli_addr, clilen);
	//r++;
	if(x == -1)
        {
            errorCall("sendto()");
        } 
	//else
	//printf("%zu\n", x);          
      }	
	i++; 
	//printf("%ld \n",r);
}
	//pthread_exit(0);
	

}

void *UdpServerData(void *val)
{
  
	struct thread_info_struct *tempData= (struct thread_info_struct*)val;
	int i=0;
        
	while(i<tempData->ops)
	{
		long int r=0;
 	  for (long int z = tempData->startIndex;z<(tempData->endIndex);z++)
	  { 
	   int x = recvfrom(tempData->sock, rec+(z*tempData->blockLength), tempData->blockLength, 0, (struct sockaddr *) &send_addr, &serlen);
	 //  if(x==-1)
          // {
           // errorCall("recvfrom()");
           //} 
	  //else
	  // printf("%d\n", x);
	  
	}
	  //printf("%ld \n",r);
	i++;
	}
}


void *UDPServerPing(void *val)
{
  
	 struct thread_info_struct *tempData= (struct thread_info_struct*)val;
	 int i=0;
	 char *ack = (char*)malloc(1);
	
	 while(i<tempData->ops)
	 {
		long int r=0;
	 	 int x = recvfrom(tempData->sock, ack, tempData->blockLength, 0, (struct sockaddr *) &send_addr, &serlen);

		 int y = sendto(tempData->sock, ack, bufferLen , 0 ,(struct sockaddr *) &send_addr, serlen);
		//printf("%s, \n", rec);
		if(y==-1)
        	{	
            	 errorCall("recvfrom()");
        	} 	  	
		i++;
	
	}
	//printf("%d \n",i);
	
}
void *UDPClientPing(void *val)
{
	struct thread_info_struct *tempData= (struct thread_info_struct*)val;
	int  i=0;
	char *ack = (char*)malloc(1);
	memset(ack, 'A',1);
        while(i<tempData->ops)
  	{
		long int r=0;  
		int x = sendto(tempData->sock, ack, bufferLen , 0 ,(struct sockaddr *) &cli_addr, clilen);      
       		int y = recvfrom(tempData->sock, ack, bufferLen, 0, (struct sockaddr *) &cli_addr, &clilen);

		if(y == -1)
        	{
            	  errorCall("sendto()");
        	} 	 
		i++;
	}
	//printf("%d \n",i);
}

void Calculation(double exec_time)
{

double theoThroughput = 10000; //chameleon website
printf("%lf \n", exec_time);
double rate = (double)(100*GB_Data*8/exec_time);
double throughput = (double)(rate/1e6);

double theoLatency =0.0007; //chameleon website
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

void UDPClient()
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
	int cliConn;
	   int totBlocks = GB_Data/bufferLen; //blocks
	   int blocksPerThread = totBlocks/totThread; //blocks per thread.
	   
	 
	    memset((char *) &cli_addr, 0, sizeof(cli_addr));
	    cli_addr.sin_family = AF_INET;
	    cli_addr.sin_port = htons(PORT);
	    //cli_addr.sin_addr.s_addr= inet_addr(ip);
	    if (inet_aton(ip , &cli_addr.sin_addr) == 0) 
	    {
		fprintf(stderr, "inet_aton() failed\n");
		exit(1);
	    }
	          gettimeofday(&startTime, NULL);
	          int count =0;
	  	    for(int j=0; j<totThread; j++)
	 	 	{ 
			    if ( (cliConn=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
	    			{
				   errorCall("socket");
	    			}
				printf("%d \n", cliConn);
			   if(bufferLen != 1)
				{
					main_thread_info[j].startIndex = j*blocksPerThread;
					main_thread_info[j].endIndex =  ((j+1)*(blocksPerThread)) - 1;
					main_thread_info[j].blockLength = bufferLen;
					main_thread_info[j].sock = cliConn;
					main_thread_info[j].ops = 100/totThread;
					pthread_create(&mythread[j],NULL,UDPClientData,&main_thread_info[j]);
					pthread_join(mythread[j],NULL);
				}
				else
				{
					main_thread_info[j].startIndex = 0;
			 		main_thread_info[j].endIndex =  0;
					main_thread_info[j].blockLength = bufferLen;
					main_thread_info[j].sock = cliConn;
					main_thread_info[j].ops = 1000000/totThread;
					pthread_create(&mythread[j],NULL,UDPClientPing,&main_thread_info[j]);
					pthread_join(mythread[j],NULL);

				}
	  		}		  
	  	//	for(int j=0; j<totThread;j++)
	  		//{	pthread_join(mythread[j],NULL);
			//}
	 //free(src);
	gettimeofday(&endTime, NULL);	
	double exec_time = (endTime.tv_sec + (endTime.tv_usec /1000000.0)) - (startTime.tv_sec + (startTime.tv_usec/ 1000000.0)); //time in sec
	Calculation(exec_time);
}

void UDPServer()
{
    pthread_t mythread[totThread];
    thread_info_struct main_thread_info[totThread];

   int totBlocks = GB_Data/bufferLen; //blocks
   int blocksPerThread = totBlocks/totThread; //blocks per thread.

    //create a UDP socket
    if ((serConn=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        errorCall("socket");
    }
     
  
    memset((char *) &ser_addr, 0, sizeof(ser_addr));
     
    ser_addr.sin_family = AF_INET;
    ser_addr.sin_port = htons(PORT);
    ser_addr.sin_addr.s_addr = htonl(INADDR_ANY);
     
    //bind socket to port
    if( bind(serConn , (struct sockaddr*)&ser_addr, sizeof(ser_addr) ) == -1)
    {
        errorCall("bind error");
    }
     printf("Listening \n");
    
      for(int j=0; j<totThread; j++)
     {
printf("%d \n", serConn);
	if(bufferLen != 1)
	{

        	main_thread_info[j].startIndex = j*blocksPerThread;
		main_thread_info[j].endIndex =  ((j+1)*(blocksPerThread)) - 1;
		main_thread_info[j].blockLength = bufferLen;
		main_thread_info[j].ops = 100/totThread;
		main_thread_info[j].sock = serConn;
		pthread_create(&mythread[j],NULL,UdpServerData, (void *) &main_thread_info[j]);
		pthread_join(mythread[j],NULL);
	}
	else
	{
		main_thread_info[j].startIndex = 0;
		main_thread_info[j].endIndex =  0;
		main_thread_info[j].blockLength = bufferLen;
		main_thread_info[j].sock = serConn;
		main_thread_info[j].ops = 1000000/totThread;
		pthread_create(&mythread[j],NULL,UDPServerPing,&main_thread_info[j]);
		pthread_join(mythread[j],NULL);
	}
     }  
  // for(int j=0; j<totThread;j++)
//	{	pthread_join(mythread[j],NULL);
//}

//free(rec);
    
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
        UDPClient();
    else 
	UDPServer();
    return 0;
}
