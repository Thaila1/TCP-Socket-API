/***TCP ITERATIVE ARCHITECTURE in C *****/
		/*** @Thaila Annamalai ***/
		

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>
#include <time.h>

#define BUFF_SIZE 4096
#define EOM "------------------------\n"

int main(int argc,char* argv[]){
	//checks for proper command line arguments
	if(argc!=2){
		printf("USAGE:./server <portnumber>\n");
		exit(1);
	}
	
	//declaring variables 
	int sockfd,ginfo,num,client_sockfd,exe,tim,len,k,sen;
	char buff[BUFF_SIZE],*name,*mes,*tok,*fname,exec_count[3],time_delay[3],lengstr[3],*command,*res,*timestamp,*timestamp1,lengStr[3],*op;
	time_t gtime;
	char clientIP[INET_ADDRSTRLEN];
	char permission[3];
	struct addrinfo hints,*serverinfo,*i;
	struct sockaddr_storage client_addr;
	int lSize, bytesRxd = 0;
	memset(&hints,0,sizeof(hints));
	hints.ai_family=AF_INET;
	hints.ai_socktype=SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	socklen_t addr_len;
	unsigned short port;
	
	//gets the information about the server port 
	if((ginfo=getaddrinfo(NULL,argv[1],&hints,&serverinfo))!=0){
		printf("Error in retrieving information\n");
		exit(1);
	}
	
	//opens the socket
	for(i=serverinfo;i!=NULL;i=i->ai_next){
			
		if((sockfd=socket(i->ai_family,i->ai_socktype,i->ai_protocol))==-1){	
			continue;
		}
		
		if(bind(sockfd,i->ai_addr,i->ai_addrlen)==-1){
			close(sockfd);
			continue;	
		}
		break;
		
	}
	
	if(i==NULL){
		printf("could not open socket\n");
		exit(1);
	}
	
	//listens for any available client
	if(listen(sockfd,1)==-1){
		printf("Not listening\n");
		exit(1);
	}

	
	while(1){
		printf("++++++++++++++++\nWaiting for connection\n");
		memset(buff,'\0',BUFF_SIZE);
		addr_len=sizeof(client_addr);
		
		//connects to the client
		client_sockfd=accept(sockfd,(struct sockaddr*)&client_addr,&addr_len);
		
		if(client_sockfd==-1){
			printf("error accepting");
			continue;
		}	
		time(&gtime);
		timestamp=ctime(&gtime);
		
		printf("%s",timestamp);
		//gets the information about the client port and client IP  
		inet_ntop(client_addr.ss_family,&((struct sockaddr_in*)&client_addr)->sin_addr,clientIP,sizeof(clientIP));
		port = ((struct sockaddr_in*)&client_addr)->sin_port;
		printf("server: Connected to client %s:%u\n",clientIP,port);
		
		int pid=fork();
		
		if(pid<0){
			printf("Error in forking");
			exit(1);
		}
		
		if(pid ==0){
			close(sockfd);
			//receives message from client
			if((num=recv(client_sockfd,buff,BUFF_SIZE-1,0))==-1){
				printf("could not receive from client\n");
				exit(1);
			}	
			printf("The received message is : %s\n",buff);
		
			//splitting the variables
			memset(lengstr,'0',3);
			strncpy(lengstr,buff,3);
			len=atoi(lengstr);
			memset(exec_count,'0',3);
			strncpy(exec_count,&buff[3],3);
			exe=atoi(exec_count);
			memset(time_delay,'0',3);
			strncpy(time_delay,&buff[6],3);
			tim=atoi(time_delay);
			command = (char*)calloc(sizeof(char),len-9);
			memset(command,'\0',len-9);
			memcpy(command,&buff[9],len-9);
		
		
			printf("The exec_count: %d\n",exe);
			printf("The tim_delay: %d\n",tim);
			printf("The command: %s\n",command);
		
			
			for(k=0;k<exe;k++){
				int j=0;
				res=(char*)calloc(BUFF_SIZE,sizeof(char));
				time(&gtime);
				FILE *fp=popen(command,"r");
				timestamp1=ctime(&gtime);
			
				if(fp==NULL){
					printf("error in executing the command");
					exit(1);
				}
				op = (char*)calloc(sizeof(char),1);
				memset(op,'\0',1);
				while(fgets(res,sizeof(res),fp)){	
					int op_len = 1+strlen(op);	
					op = (char*)realloc(op,op_len+strlen(res));
					strcat(op,res);
				}	
				pclose(fp);
				lSize = strlen(op);
				printf("LSIze:%d\n",lSize);
				long mes_size = lSize + 5 + strlen(timestamp1);
				int pkts = (mes_size) / BUFF_SIZE;
				if ((mes_size)%BUFF_SIZE != 0)
					pkts++;
				sprintf(lengStr,"%.3d",lSize);
			
				mes=(char*)calloc(sizeof(char),pkts*BUFF_SIZE);
				memset(mes,'\0',mes_size);			
				strcpy(mes,lengStr);
				strcat(mes," ");
				strcat(mes,timestamp1);
				strcat(mes,"\n");
				strcat(mes,op);
				//printf("The message to be sent:%s",mes);
				printf("Execution count: %d\n",k+1);
				printf("Time: %s\n",timestamp1);
				printf("Output:\n%s\n",op);
				while(pkts){			
					//sending the data to the client
					if(send(client_sockfd,mes,mes_size,0)==-1){
						printf("message not sent\n");
						close (client_sockfd);
						exit(1);
	   				}
					j += BUFF_SIZE;
					pkts--;			
				}
				if (k != exe -1)
					sleep(tim);					
			}	
			if (mes != NULL){
				free(mes);
				mes = NULL;
			}
			if (res != NULL){
				free(res);
				res = NULL;
			}
			if (op != NULL){
				free(op);
				op = NULL;
			}
			printf("STATUS:connection Terminating with the client.\n");
			printf("%s",EOM);
			printf("Waiting for connection\n");
			exit(1);
			
		}		
		else{
			close(client_sockfd);
		}
		
		
	}
	close(sockfd);
	return 0;
}

