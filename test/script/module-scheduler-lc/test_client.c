#include    <stdio.h>
#include    <stdlib.h>
#include    <strings.h>
#include    <sys/types.h>
#include    <sys/socket.h>
#include    <netinet/in.h>
#include    <arpa/inet.h>
#include    <netdb.h>
#include    <signal.h>

int sockfd;

int main(int argc, char *argv[])
{
    void sigend(void);
    struct  sockaddr_in server;

    sockfd = socket(PF_INET, SOCK_STREAM, 0);
    bzero((char *)&server, sizeof(server));
    server.sin_family = PF_INET;
    server.sin_port   = htons(atoi(argv[2]));
    server.sin_addr.s_addr = inet_addr(argv[1]);
    if( connect(sockfd, (struct sockaddr *)&server, sizeof(server)) == -1 ){
        perror("connect failed");
        exit(1);
    }

    if( signal(SIGTERM, (void *)sigend) == SIG_ERR){
	perror("signal failed");
	exit(1);
    }

    sleep(10);
    
    close(sockfd);
    return(0);
}

void sigend(void){
	close(sockfd);
	exit(0);
}

