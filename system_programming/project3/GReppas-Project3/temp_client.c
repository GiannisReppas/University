#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <netdb.h>
#include <ctype.h>
#include <signal.h>
#include <arpa/inet.h>
#include <errno.h>

#include "itoa_plus_nod.h"

int main(int argc, char *argv[])
{
	int port, sock, i;
	char buf[256];
	struct sockaddr_in server;
	struct sockaddr *serverptr = (struct sockaddr*) &server;
	struct hostent *rem;

	if ( argc != 4)
	{
		printf("Please give host name and port number\n");
		exit(1);
	}

	bool check=true;
	char temp[256];
	do
	{
		/* Create socket */
		if ( (sock = socket(AF_INET,SOCK_STREAM,0)) < 0)
		{
			printf("Error with socket\n");
			exit(1);
		}

		/* Find server address */
		if ( (rem = gethostbyname(argv[1])) == NULL)
		{
			printf("Error with gethostbyname\n");
			exit(1);
		}

		port = atoi(argv[2]); /* Convert port number into integer */
		server.sin_family = AF_INET; /* Internet domain */
		memcpy(&server.sin_addr,rem->h_addr,rem->h_length);
		server.sin_port = htons(port); /* server port */

		/* Connect */
		if ( connect(sock,serverptr,sizeof(server)) < 0)
		{
			printf("Error with connect\n");
			exit(1);
		}

		if (check) printf("Connecting to %s port %d\n",argv[1],port);
		check = false;

		// pass
		char hostbuffer[256];
		struct hostent *rem2;
		gethostname(hostbuffer,sizeof(hostbuffer));
		rem2 = gethostbyname(hostbuffer);

		char *ip_buffer;
		ip_buffer = inet_ntoa( *((struct in_addr*)rem2->h_addr));
		puts(ip_buffer);

		printf("Give input : \n");
		fgets(buf,sizeof(buf),stdin); /* Read from stdin */

		i=0;
		while (buf[i] != '\n') i++;
		buf[i] = '\0';

		strcpy(temp,buf);

		struct in_addr ip_struct;
		inet_aton(ip_buffer,&ip_struct);
		char *ip_network_form=malloc(sizeof(char)*number_of_digits(ip_struct.s_addr) + sizeof(char));
		integer_to_string(ip_struct.s_addr,ip_network_form);
		strcat(buf," ");
		strcat(buf,ip_network_form);

		char *port_string=malloc(sizeof(char)*number_of_digits(htonl(89)) + sizeof(char));
		integer_to_string(htonl(atoi(argv[3])),port_string);
		strcat(buf," ");
		strcat(buf,port_string);

		if ( send(sock,buf,strlen(buf)+sizeof(char),0) < 0)
		{
			printf("Error with write\n");
			exit(1);
		}

		if ( !strcmp(temp,"GET_CLIENTS") )
		{
			char *buf2;
			buf2 = malloc(sizeof(char)*13);
			if ( recv(sock,buf2,sizeof(char)*13,0) < 0)
			{
				printf("Error with read\n");
				exit(1);
			}
			puts(buf2);
			free(buf2);

			int temp;
			if ( recv(sock,&temp,sizeof(int),0) < 0)
			{
				printf("Error with recv\n");
				exit(1);
			}

			buf2 = malloc(sizeof(char));
			if ( recv(sock,buf2,sizeof(char),0) < 0)
			{
				printf("Error with recv\n");
				exit(1);
			}

			int i,temp2;
			for ( i=0; i<temp; i++)
			{
				while( recv(sock,buf2,sizeof(char),0) > 0)
				{
					putchar(buf2[0]);

					if ( buf2[0] == ' ') break;
				}

				putchar('\n');

				if ( recv(sock,&temp2,sizeof(int),0) < 0)
				{
					printf("Error with recv\n");
					exit(1);
				}

				printf("%d\n",temp2);

				putchar('\n');
			}
			free(buf2);
		}
else {
		close(sock); /* Close socket */
if ( atoi(argv[3]) == 9004) break;
		if ( (sock = socket(AF_INET,SOCK_STREAM,0)) < 0)
		{
			printf("Error with socket\n");
			exit(1);
		}

		struct sockaddr_in server2;
		struct sockaddr *serverptr2;
		server2.sin_family = AF_INET;
		server2.sin_addr.s_addr = htonl(INADDR_ANY);
		server2.sin_port = htons(atoi(argv[3]));
		serverptr2 = (struct sockaddr* ) &server2;

		if ( bind(sock,serverptr2,sizeof(server2)) <0 )
		{
			printf("Error with bind - %s\n",strerror(errno));
			exit(1);
		}

		if ( listen(sock,10) < 0)
		{
			printf("Error with listen\n");
			exit(1);
		}

		int newsock;
		struct sockaddr_in client;
		struct sockaddr *clientptr = (struct sockaddr*) &client;
		int clientlen = sizeof(client);
		if ( (newsock = accept(sock,clientptr,&clientlen)) < 0)
		{
			printf("Error with accept\n");
			exit(1);
		}

		int space_counter = 0;
		char *result=malloc(sizeof(char));
		if ( result == NULL) { printf("Error with malloc\n"); exit(1); }
		result[0] = '\0';
		char ggg[1];
		while ( recv(newsock,ggg,1,0) > 0 )
		{
			if( ggg[0] == ' ') space_counter++;

			if (space_counter == 3) break;

			result = realloc(result, sizeof(char)*strlen(result) + sizeof(char)*2);
			result[strlen(result)+1] = '\0';
			result[strlen(result)] = ggg[0];
		}

		puts(result);

		close(newsock);
}
break;
	} while( strcmp(temp,"LOG_OFF") );

	return 0;
}
