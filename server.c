#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#define True 1
#define PORT 10000

void write_all(int fd,char* buf,int len){
	while(len>0){
		int n=write(fd,buf,len);
		if(n<=0) break;
		buf+=n;
		len-=n;
	}
}

void handle_request(int fd){

	char buf[0x1000+1];
	int total = read(fd,buf,0x1000);
	buf[total] = 0;
	char* method = strtok(buf," ");
	char* path = strtok(NULL," ");
	char* http_version = strtok(NULL,"\r\n");

	char rpath[0x1000]=".";
	if(strcmp(path,"/")==0) strcat(rpath,"/index.html");
	else strcat(rpath,path);
	int ffd = open(rpath,O_RDONLY);
	char respone[0x1000];
	if(ffd>0){
		sprintf(respone,
		"%s 200 OK\r\n"
		"Content-Type: text/html\r\n"
		"Content-Length: %ld\r\n\r\n",http_version,lseek(ffd,0,SEEK_END));
		lseek(ffd,0,SEEK_SET);
		int len = strlen(respone);
		write_all(fd,respone,len);
		while(True){
			int n=read(ffd,buf,0x1000);
			if(n<=0) break;
			write_all(fd,buf,n);
		}
		close(ffd);
	}
	else{
		strcat(http_version,
		" 404 Not Found\r\n"
		"Content-Type: text/html\r\n"
		"Content-Length: 9\r\n\r\n"
		"Not Found");
		int len = strlen(http_version);
		write_all(fd,http_version,len);
	}
	
	
	//HTTP/1.1 200 OK
	//Content-Type: text/html
	//Content

	//HTTP/1.1 404 Not Found
	//Content-Type: text/html
	//Content	
	
	
}

int main(int argc,char** argv){
	

	if(argc<2){
		puts("./server [port}");
		return 0;
	}
	int sockfd;
	struct sockaddr_in server_addr;

	if((sockfd=socket(AF_INET,SOCK_STREAM,0)) < 0){
		perror("can't opne sock\n");
		exit(-1);
	}
	bzero((char*)&server_addr,sizeof(server_addr));
	server_addr.sin_family=AF_INET;
	server_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	server_addr.sin_port = htons(atoi(argv[1]));

	if(bind(sockfd,(struct sockaddr*)
		&server_addr,sizeof(server_addr)) < 0){
		perror("can't bind\n");
        exit(-1);
	}

	listen(sockfd,5);
	printf("Listen on PORT %d\n",PORT);
	while(1){
		int clientfd,childpid;
		struct sockaddr_in client_addr;
		unsigned int addrlen = sizeof(client_addr);
		clientfd = accept(sockfd,(struct sockaddr*)&client_addr,&addrlen);
		handle_request(clientfd);
		close(clientfd);
	}

}
