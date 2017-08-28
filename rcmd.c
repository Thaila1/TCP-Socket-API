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

#define BUFF_SIZE 4096
#define EOM "------------------------\n"

int main(int argc,char* argv[]){
	//checks for proper command line arguments
	if(argc<6){
		printf("USAGE: ./rcmdd <server_address> <port_number> <Execution_time> <time_delay> <command>\n");
		exit(1);
	}
	
	//declaration of all socket variables				
	int sockfd,ginfo,num,exe,tim,len,j,rxd,k,cmd_len = 0;
	char *buff,exec_count[3],time_delay[3],*command,*mes,lengstr[3];
	struct addrinfo hints,*serverinfo,*i;
	
	memset(&hints,0,sizeof(hints));
	hints.ai_family=AF_INET;
	hints.ai_socktype=SOCK_STREAM;
	
	//gets the information about the server port and server IP  
	if((ginfo=getaddrinfo(argv[1],argv[2],&hints,&serverinfo))!=0){
		printf("Error in retriving information from IP address\n");
		exit(1);
	}
	
	//opening socket
	for(i=serverinfo;i!=NULL;i=i->ai_next){
		if((sockfd=socket(i->ai_family,i->ai_socktype,i->ai_protocol))==-1){
			continue;
		}
		
		if(connect(sockfd,i->ai_addr,i->ai_addrlen)==-1){
			printf("Not connected");
			close(sockfd);
			continue;
		}
		break;
	}	
	
	if(i==NULL){
		printf("Socket failed to open\n");
		return EXIT_FAILURE;
	}
	
	//form the message to be sent
	strcpy(exec_count,argv[3]);
	strcpy(time_delay,argv[4]);
	for (k = 5; k < argc; k++)
		cmd_len += strlen(argv[k])+1;
	
	command= (char*)calloc(sizeof(char),cmd_len);
	memset(command,'\0',cmd_len);
	
	exe=atoi(exec_count);
	tim=atoi(time_delay);
	sprintf(exec_count,"%.3d",exe);
	sprintf(time_delay,"%.3d",tim);
	strcpy(command,argv[5]);
	for (k = 6; k <argc;k++){
		strcat(command," ");
		strcat(command,argv[k]);
	}
	len=strlen(exec_count)+strlen(time_delay)+strlen(command)+3;
	sprintf(lengstr,"%.3d",len);
	
	mes=(char*)calloc(len,sizeof(char));
	strcpy(mes,lengstr);
	strcat(mes,exec_count);
	strcat(mes,time_delay);
	strcat(mes,command);
	
	printf("The message to be sent is: %s\n",mes);
	printf ("Message sent to server\n\n");
	
	if(send(sockfd,mes,len,0)==-1){
		printf("message not sent\n");
		close(sockfd);
		exit(1);

	}
	buff = (char*)calloc(sizeof(char),BUFF_SIZE);
	memset(buff,'\0',BUFF_SIZE);
	while(exe>0){
		if((num=recv(sockfd,buff,BUFF_SIZE,0))==-1){
			printf("could not receive from server\n");
			exit(1);
		}
		//finds the length of the data 
		strncpy(lengstr,buff,3);
		len=atoi(lengstr);
		
		//forms packet to accomodate the data
		int pkt=len/BUFF_SIZE;
		
		if(len%BUFF_SIZE!=0)
			pkt++;
		//printf("\npakets:%d\n",pkt);
		buff=(char*)realloc(buff,pkt*BUFF_SIZE);
		j=BUFF_SIZE;
		rxd = 1;
		
		while(pkt>1){
			if((num=recv(sockfd,&buff[j],BUFF_SIZE,0))==-1){
				printf("response not received from server\n");
				rxd = 0;
			}
			j+=BUFF_SIZE;
			pkt--;
			rxd++;
		}		
		
		if (rxd == 0)
			continue;
		
		printf("Server response:\n%s\n",buff);
		buff=(char*)realloc(buff,BUFF_SIZE);
		memset(buff,'\0',BUFF_SIZE);
		exe--;
	}
	free(command);
	free(mes);	
	freeaddrinfo(serverinfo);
	return 0;
}
	
	
	
	
