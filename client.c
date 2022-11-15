#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <time.h>

#define N_OF_PORT 5
#define MESS_SIZE 65536

void ErrorHandling(char* message);
void* thread_socket(void* data);

typedef struct thread_data{
    char addr[255];
    int port;
} thread_data;

int main(int argc, char const *argv[]){
	int servPorts[N_OF_PORT];
	int tid;
	char addr[255];
	pthread_t thread[N_OF_PORT];
	thread_data data[N_OF_PORT];

	printf("Type Server IP Address: ");
	scanf("%s", addr);
	printf("Type Server Ports(format:<Port1> <Port2> <Port3> <Port4> <Port5>): \n");
	scanf("%d %d %d %d %d", &servPorts[0], &servPorts[1], &servPorts[2], &servPorts[3], &servPorts[4]);

	for(int i = 0; i < N_OF_PORT; i++){
		strcpy(data[i].addr, addr);
		data[i].port = servPorts[i];
		tid = pthread_create(&thread[i], NULL, thread_socket,(void *) &data[i]);
		if (tid < 0) ErrorHandling("Thread create error\n");
}	
	
	pthread_join(thread[0], NULL);
	pthread_join(thread[1], NULL);
	pthread_join(thread[2], NULL);
	pthread_join(thread[3], NULL);
	pthread_join(thread[4], NULL);

	return 0;

}

void* thread_socket(void* data){
	thread_data *serv = (thread_data*) data;
	int clntSock, servPort;
	struct sockaddr_in servAddr;
	char message[MESS_SIZE];
	char timeBuff[20];
	struct tm* time_info;
	struct timeval tv;
	int millisec;
	char filename[10];
	FILE *fp;
	
	//socket create
	clntSock = socket(AF_INET, SOCK_STREAM, 0);
	if(clntSock == -1) ErrorHandling("socket() error\n");

	//connect
	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(serv->port);

	if(inet_pton(AF_INET, serv->addr, &servAddr.sin_addr) <= 0) ErrorHandling("Invalid address\n");

	if(connect(clntSock, (struct sockaddr*)&servAddr, sizeof(servAddr))<0) ErrorHandling("Connection failed\n");

	//receive message from server
	if( recv(clntSock , message , MESS_SIZE , 0) < 0) ErrorHandling("recv() failed\n");

	gettimeofday(&tv, NULL);
	millisec = lrint(tv.tv_usec/1000.0); // Round to nearest millisec
  	if (millisec>=1000) { 
    		millisec -=1000;
    		tv.tv_sec++;
  	}
	time_info = localtime(&tv.tv_sec);
	strftime(timeBuff, 20, "%H:%M:%S", time_info);

	sprintf(filename, "%d", serv->port);
	strcat(filename, ".txt");

	fp = fopen(filename, "a");
	if(fp == NULL) ErrorHandling("Error\n");

	fprintf(fp, "%s.%d %d %s\n", timeBuff, millisec, strlen(message), message);
	fclose(fp);
	
	close(clntSock);
}

void ErrorHandling(char* message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}