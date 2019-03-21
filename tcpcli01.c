// ***** CLIENT *****

#include	"unp.h"
#include <string.h>

ssize_t n; // size for read

void
str_cli(FILE *fp, int sockfd)
{
	char	sendline[MAXLINE], recvline[MAXLINE]; // buffers

	printf("\n\n\t\tCAMERON UNIVERSITY BANK\n\n");
	printf("Use the following commands to interact:\n\n");
	printf("1:	login\n");
	printf("2:	Create an account\n");
	printf("\nEnter your Command: ");
	while (fgets(sendline, MAXLINE, fp) != NULL) { // get sendline

		//strtok(sendline, "\n");
		write(sockfd, sendline, strlen(sendline)); // write to server
		
		if ((n = read(sockfd, recvline, MAXLINE)) == 0) // not receiving messages
		{
			// handle error
			printf("str_cli: server terminated prematurely\n");//Thank you for using CU BANK!
			return;
		}
		recvline[n] = 0; // null terminate the buffer
		fputs(recvline, stdout); // output recieved text
	}
}

int
main(int argc, char **argv)
{
	int					sockfd; // file descriptor
	struct sockaddr_in	servaddr; // server address structure

	if (argc != 2) // insufficient args
	{
		// handle error
		printf("usage: tcpcli <IPaddress>");
		return -1;
	}

	sockfd = socket(AF_INET, SOCK_STREAM, 0); // create socket

	bzero(&servaddr, sizeof(servaddr)); // zero out addr
	servaddr.sin_family = AF_INET; // set family
	servaddr.sin_port = htons(SERV_PORT); // set port
	inet_pton(AF_INET, argv[1], &servaddr.sin_addr); 

	connect(sockfd, (SA *) &servaddr, sizeof(servaddr)); // connect to server

	str_cli(stdin, sockfd);		/* do it all */

	exit(0);
}
