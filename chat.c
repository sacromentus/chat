//Jerry Sufleta HW # 7

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <signal.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>

#define HOST	"cs.bc.edu"
#define PORT	8120
#define MAXLINE	1000


int open_clientfd(char *hostname, int port);
void parler();
void ecouter();

int clientfd;	//This is the index of our connection to the chat server
int PID;	//Holds the process id of the child; we make it global so we can kill the child upon exit

int main() {
	
	clientfd = open_clientfd(HOST, PORT);

	if(clientfd < 0) {
		fprintf(stderr, "Unable to connect to %s:%d", HOST, PORT);
		exit(4);
	}

	printf("\aWelcome to J-Chat (type /exit to quit)\n");

	if(PID = fork())	//Is this the parent process
		parler();
		
	else 
		ecouter();
	
	close(clientfd);
	
	return 0;
}

void ecouter() {
	int length;
	char buf[MAXLINE];

	while( (length = read(clientfd, buf, MAXLINE-1))) {
		buf[length] = '\0';
		fputs(buf, stdout);
	}
} 

void parler() {
	int length;
	char buf[MAXLINE];

	do {
		fgets(buf, MAXLINE-1, stdin);		

		if(strcmp("/exit\n", buf) == 0) {	//user wants to exit program
			kill(PID, SIGKILL);	//first shut down the child process
			close(clientfd);
			exit(0);		//OK to exit
		}

		length = write(clientfd, buf, strlen(buf)+1);

		if(length < 0) 
			fprintf(stderr, "Error writing to socket");
	}while(1);			
}

int open_clientfd(char *hostname, int port) {
	int clientfd;
	struct hostent *hp;
	struct sockaddr_in *serveraddr = calloc(1, sizeof(struct sockaddr_in)); // use calloc, need zero'd

	// ** create socket
	if ((clientfd = socket(AF_INET, SOCK_STREAM, 0))<0) {
		fprintf(stderr, "Error creating socket\n");
		free(serveraddr);
		return -1;
	}

	// ** Fill in the server's IP address and port
	if (!(hp = gethostbyname(hostname))) {
		fprintf(stderr, "Unable to get IP address of host %s\n", hostname);
		free(serveraddr);
		return -1;
	}
	serveraddr->sin_family = AF_INET;
	serveraddr->sin_port = htons(port);
	bcopy((char *)hp->h_addr_list[0],
	      (char *)&serveraddr->sin_addr.s_addr, hp->h_length);

	// ** Finally, establish the connection
	if (connect(clientfd, (struct sockaddr *)serveraddr, sizeof(struct sockaddr_in)) < 0) {
		fprintf(stderr, "Unable to connect to server %s\n", hostname);
		free(serveraddr);
		return -1;
	}
	free(serveraddr);
	return clientfd; // success
}
