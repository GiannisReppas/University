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
#include <pthread.h>
#include <errno.h>
#include <arpa/inet.h>
#include <poll.h>

#include "itoa_plus_nod.h"

//////////////////////////////////////////////////////////////////////////////////////

// a node for the list of the loged clients
typedef struct ListNode
{
	char *ip_address;
	int port_num;
	struct ListNode *next;
}ListNode;

// struct that will be passed to each thread
typedef struct ForThread
{
	int new_sock;
	char *ip_address;
}ForThread;

//////////////////////////////////////////////////////////////////////////////////////

// mutex for thread
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

// listhead
ListNode *head = NULL;

//////////////////////////////////////////////////////////////////////////////////////

// function for LOG_ON
void log_on( char *ip, int port, int newsocket)
{
	// lock mutex, going to work with the list
	pthread_mutex_lock(&lock);

	// cretae the new node
	ListNode *newNode;
	newNode = malloc(sizeof(ListNode));
	if (newNode == NULL) { printf("Error with malloc\n"); exit(1); }
	newNode->ip_address = malloc(sizeof(char)*strlen(ip) + sizeof(char));
	if (newNode->ip_address == NULL) { puts("Error with malloc"); exit(1); }
	strcpy(newNode->ip_address,ip);
	newNode->port_num = port;
	newNode->next = NULL;

	// put the new node in the list, if it already exists, print the error and end log_on() function
	if ( head == NULL)
	{
		head = newNode;
	}
	else
	{
		bool check = false;
		ListNode *temp=head;
		while (1)
		{
			if ( !strcmp(temp->ip_address,ip) && (temp->port_num == port) )
			{
				check = true;
				break;
			}

			if ( temp->next == NULL)
			{
				break;
			}

			temp = temp->next;
		}

		if (check == false)
		{
			temp->next = newNode;
		}
		else
		{
			printf("LOG_ON already executed with same ip address or same port\n");

			pthread_mutex_unlock(&lock);

			return;
		}
	}

	// USER_ON
	// now, send message back to client
	int another_sock; // new sock for client

	struct sockaddr_in client;
	struct sockaddr *clientptr= (struct sockaddr *) &client;
	struct hostent *rem;

	char *buf;
	ListNode *temp=head;

	char *string_port;
	char *ip_network_form;

	// for every list node except the last one that was just inserted
	while( temp->next != NULL)
	{
		// create new socket
		if ( (another_sock = socket(AF_INET,SOCK_STREAM,0)) < 0)
		{
			printf("Error with socket\n");
			exit(1);
		}

		client.sin_family = AF_INET;
		inet_pton(AF_INET,temp->ip_address,&client.sin_addr);

		client.sin_port = htons(temp->port_num);

		// connect with client
		if ( connect(another_sock,clientptr,sizeof(client)) < 0)
		{
			printf("Error with connect - %s\n",strerror(errno));
			exit(1);
		}

		// buffer is an array with the whole message
		buf = malloc(sizeof(char)*9);
		if ( buf == NULL) { printf("Error with malloc\n"); exit(1); }
		strcpy(buf,"USER_ON ");

		struct in_addr ip_struct;
		inet_aton(newNode->ip_address,&ip_struct);
		ip_network_form = malloc(sizeof(char)*number_of_digits(ip_struct.s_addr)+sizeof(char));
		if ( ip_network_form == NULL) { printf("Error with malloc\n"); exit(1); }
		integer_to_string(ip_struct.s_addr,ip_network_form);
		buf = realloc( buf, strlen(buf) + strlen(ip_network_form) + 2*sizeof(char) );
		if ( buf == NULL) { printf("Error with realloc\n"); exit(1); }
		strcat(buf,ip_network_form);
		strcat(buf," ");

		string_port = malloc(sizeof(char)*number_of_digits(htonl(newNode->port_num)));
		if (string_port == NULL) { printf("Error with malloc\n"); exit(1); }
		integer_to_string(htonl(newNode->port_num),string_port);
		buf = realloc(buf, strlen(buf) + strlen(string_port) + 2*sizeof(char));
		if ( buf == NULL) { printf("Error with realloc\n"); exit(1); }
		strcat(buf,string_port);
		strcat(buf," ");

		// send buffer to client
		if ( send(another_sock,buf,strlen(buf)+sizeof(char),0) < 0)
		{
			printf("Error with send\n");
			exit(1);
		}

		// free memory
		free(ip_network_form);
		free(string_port);
		free(buf);

		// next client
		temp = temp->next;

		// close the socket you just created
		close(another_sock);
	}

	// end with LOG_ON and with list changes, unlock mutex
	pthread_mutex_unlock(&lock);

	return;
}

// function for GET_CLIENTS
void get_clients( int sock)
{
	// lock mutex, going to work with the list
	pthread_mutex_lock(&lock);

	// find list length
	int count=0;
	ListNode *temp=head;
	while( temp != NULL)
	{
		count++;

		temp = temp->next;
	}

	// send CLIENT LIST N
	char *buf;
	buf = malloc(sizeof(char)*13);
	if ( buf == NULL) { printf("Error with malloc\n"); exit(1); }
	strcpy(buf,"CLIENT_LIST ");
	if ( send(sock,buf,sizeof(char)*13,0) < 0)
	{
		printf("Error with send\n");
		exit(1);
	}
	free(buf);

	if ( send(sock,&count,sizeof(int),0) < 0)
	{
		printf("Error with send\n");
		exit(1);
	}

	buf = malloc(sizeof(char));
	if (buf == NULL) { printf("Error with malloc\n"); exit(1); }
	buf[0] = ' ';
	if ( send(sock,buf,sizeof(char),0) < 0)
	{
		printf("Error with send\n");
		exit(1);
	}

	// for every client in the list
	temp = head;
	while( temp != NULL)
	{
		if ( send(sock,temp->ip_address,strlen(temp->ip_address),0) < 0)
		{
			printf("Error with send\n");
			exit(1);
		}

		buf[0] = ' ';
		if ( send(sock,buf,sizeof(char),0) < 0)
		{
			printf("Error with send\n");
			exit(1);
		}

		if ( send(sock,&(temp->port_num),sizeof(int),0) < 0)
		{
			printf("Error with send\n");
			exit(1);
		}

		temp = temp->next;
	}

	// end with GET_CLIENTS and with list changes, unlock mutex
	pthread_mutex_unlock(&lock);

	return;
}

// function for LOG_OFF
void log_off( char *passed_ip_address)
{
	// lock mutex, going to work with the list
	pthread_mutex_lock(&lock);

	// for every client in list
	ListNode *previous=NULL;
	ListNode *current=head;
	while(current != NULL)
	{
		// if the current node is the one we want to remove
		if (!strcmp(current->ip_address,passed_ip_address))
		{
			// keed port and ip in some local variables beq the node will be deleted
			int portToSend = current->port_num;
			char *ipToSend = malloc(sizeof(char)*strlen(current->ip_address) + sizeof(char));
			if ( ipToSend == NULL) { printf("Error with malloc\n"); exit(1); }
			strcpy(ipToSend,current->ip_address);

			// delete the node
			if ( previous == NULL)
			{
				head = NULL;
				free(current);
			}
			else
			{
				previous->next = current->next;
				free(current);
			}

			// USER_OFF
			int another_sock;

			struct sockaddr_in client;
			struct sockaddr *clientptr= (struct sockaddr *) &client;
			struct hostent *rem;

			char *buf;
			ListNode *temp=head;

			char *string_port;
			char *ip_network_form;

			// for every client in list
			while( temp != NULL)
			{
				// create new socket
				if ( (another_sock = socket(AF_INET,SOCK_STREAM,0)) < 0)
				{
					printf("Error with socket\n");
					exit(1);
				}

				client.sin_family = AF_INET;
				inet_pton(AF_INET,temp->ip_address,&client.sin_addr);
				client.sin_port = htons(temp->port_num);

				// connect with client
				if ( connect(another_sock,clientptr,sizeof(client)) < 0)
				{
					printf("Error with connect - %s\n",strerror(errno));
					exit(1);
				}

				buf = malloc(sizeof(char)*10);
				if ( buf == NULL) { printf("Error with malloc\n"); exit(1); }
				strcpy(buf,"USER_OFF ");

				struct in_addr ip_struct;
				inet_aton(ipToSend,&ip_struct);
				ip_network_form = malloc(sizeof(char)*number_of_digits(ip_struct.s_addr)+sizeof(char));
				if ( ip_network_form == NULL) { printf("Error with malloc\n"); exit(1); }
				integer_to_string(ip_struct.s_addr,ip_network_form);
				buf = realloc( buf, strlen(buf) + strlen(ip_network_form) + 2*sizeof(char) );
				if ( buf == NULL) { printf("Error with realloc\n"); exit(1); }
				strcat(buf,ip_network_form);
				strcat(buf," ");

				string_port = malloc(sizeof(char)*number_of_digits(htonl(portToSend)));
				if (string_port == NULL) { printf("Error with malloc\n"); exit(1); }
				integer_to_string(htonl(portToSend),string_port);
				buf = realloc(buf, strlen(buf) + strlen(string_port) + 2*sizeof(char));
				if ( buf == NULL) { printf("Error with realloc\n"); exit(1); }
				strcat(buf,string_port);
				strcat(buf," ");

				// send info
				if ( send(another_sock,buf,strlen(buf)+sizeof(char),0) < 0)
				{
					printf("Error with send\n");
					exit(1);
				}

				free(ip_network_form);
				free(string_port);
				free(buf);

				temp = temp->next;

				close(another_sock);
			}

			free(ipToSend);

			// end with LOG_OFF and with list changes, unlock mutex
			pthread_mutex_unlock(&lock);

			return;
		}

		previous = current;
		current = current->next;
	}

	// searched all list without finding the given input

	// end with LOG_OFF and with list changes, unlock mutex
	pthread_mutex_unlock(&lock);

	printf("ERROR_IP_PORT_NOT_FOUND_IN_LIST\n");

	return;
}

void *child_server_thread( void *arg)
{
	// read struct that was passed
	int socket = ((ForThread *) arg)->new_sock;
	char *passed_ip_address = malloc( sizeof(char)*strlen( (char *) ((ForThread *) arg)->ip_address ) + sizeof(char));
	if (passed_ip_address == NULL) { printf("Error with malloc\n"); exit(1); }
	strcpy(passed_ip_address,(char *) ((ForThread *) arg)->ip_address);

	// read first word in socket
	char *buffer;
	buffer = malloc(sizeof(char));
	if (buffer == NULL) { printf("Error with malloc\n"); exit(1); }
	buffer[0] = '\0';
	char temp_buf[1];
	bool check;

	check = false;
	while ( (recv(socket,temp_buf,1,0) > 0) )
	{
		buffer = realloc( buffer, strlen(buffer)+2);
		if (buffer == NULL) { printf("Error with realloc\n"); exit(1); }
		buffer[strlen(buffer)+1] = '\0';
		buffer[strlen(buffer)] = temp_buf[0];

		if ( temp_buf[0] == '\0') break;
	}

	char *first_word_of_buffer;
	first_word_of_buffer = malloc(sizeof(char));
	if (first_word_of_buffer == NULL) { printf("Error with malloc\n"); exit(1); }
	first_word_of_buffer[0] = '\0';
	int i=0;
	while ( (buffer[i] != '\0') && (buffer[i] != ' ') && (buffer[i] != '\n') && (buffer[i] != '\t') )
	{
		first_word_of_buffer = realloc( first_word_of_buffer, strlen(first_word_of_buffer)+2);
		if (first_word_of_buffer == NULL) { printf("Error with realloc\n"); exit(1); }
		first_word_of_buffer[strlen(first_word_of_buffer)+1] = '\0';
		first_word_of_buffer[strlen(first_word_of_buffer)] = buffer[i];

		i++;
	}

	// for first word == "LOG_ON"
	if ( (!strcmp(first_word_of_buffer,"LOG_ON")) && (buffer[i] != '\0') )
	{
		// read second word
		char *second_word_of_buffer;
		second_word_of_buffer = malloc(sizeof(char));
		if ( second_word_of_buffer == NULL) { printf("Error with malloc\n"); exit(1); }
		second_word_of_buffer[0] = '\0';

		i++;
		while ( (buffer[i] != '\0') && (buffer[i] != ' ') && (buffer[i] != '\n') && (buffer[i] != '\t') )
		{
			second_word_of_buffer = realloc( second_word_of_buffer, strlen(second_word_of_buffer)+2);
			if (second_word_of_buffer == NULL) { printf("Error with realloc\n"); exit(1); }
			second_word_of_buffer[strlen(second_word_of_buffer)+1] = '\0';
			second_word_of_buffer[strlen(second_word_of_buffer)] = buffer[i];

			i++;
		}

		// read third word
		char *third_word_of_buffer;
		third_word_of_buffer = malloc(sizeof(char));
		if ( third_word_of_buffer == NULL) { printf("Error with malloc\n"); exit(1); }
		third_word_of_buffer[0] = '\0';

		i++;
		while ( (buffer[i] != '\0') && (buffer[i] != ' ') && (buffer[i] != '\n') && (buffer[i] != '\t') )
		{
			third_word_of_buffer = realloc( third_word_of_buffer, strlen(third_word_of_buffer)+2);
			if (third_word_of_buffer == NULL) { printf("Error with realloc\n"); exit(1); }
			third_word_of_buffer[strlen(third_word_of_buffer)+1] = '\0';
			third_word_of_buffer[strlen(third_word_of_buffer)] = buffer[i];

			i++;
		}

		printf("Received LOG_ON\n");

		struct in_addr ip_struct; ip_struct.s_addr = atoi(second_word_of_buffer);

		log_on( inet_ntoa(ip_struct), ntohl(atoi(third_word_of_buffer)), socket);

		free(third_word_of_buffer);
		free(second_word_of_buffer);
	}
	// for first word == "GET_CLIENTS"
	else if ( !strcmp(first_word_of_buffer,"GET_CLIENTS") )
	{
		printf("Received GET_CLIENTS\n");

		get_clients( socket);
	}
	// for first word == "LOG_OFF"
	else if ( !strcmp(first_word_of_buffer,"LOG_OFF") )
	{
		printf("Received LOG_OFF\n");

		log_off( passed_ip_address);
	}
	// otherwise, wrong input
	else
	{
		printf("There is no such command for the server (%s) - error\n",first_word_of_buffer);
	}

	free(first_word_of_buffer);
	free(buffer);

	close(socket);
}

void command_line_check( int argc, char **argv, int *port)
{
	if ( argc != 3)
	{
		printf("Error with number of arguements at command line\n");
		exit(1);
	}

	if ( strcmp(argv[1],"-p") )
	{
		printf("Second arguement at command line must be -p");
		exit(1);
	}

	*port = atoi(argv[2]);

	return;
}

int main( int argc, char *argv[])
{
	int port;
	command_line_check( argc, argv, &port);

	/* Create socket */
	int sock;
	if ( (sock = socket(AF_INET,SOCK_STREAM,0)) < 0)
	{
		printf("Error with socket\n");
		exit(1);
	}

	struct sockaddr_in server;
	struct sockaddr *serverptr = (struct sockaddr*)&server;
	struct sockaddr_in client;
	struct sockaddr *clientptr = (struct sockaddr*)&client;
	server.sin_family = AF_INET; /* Internet domain */
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(port);

	/* Bind socket to adress */
	if ( bind(sock,serverptr,sizeof(server)) <0 )
	{
		printf("Error with bind - %s\n",strerror(errno));
		exit(1);
	}

	/* Listen to connections */
	if ( listen(sock,10) < 0)
	{
		printf("Error with listen\n");
		exit(1);
	}
	printf("Listening for connections to port %d\n",port);

	socklen_t clientlen;
	int newsock;
	struct hostent *rem;
	pthread_t thr;
	ForThread to_pass;
	while (1)
	{
		clientlen = sizeof(client);
		/* accept connection */
		if ( (newsock = accept(sock,clientptr,&clientlen)) < 0)
		{
			printf("Error with accept - %s\n",strerror(errno));
			exit(1);
		}

		/* Find clients name */
		if ( (rem = gethostbyaddr((char*)&client.sin_addr.s_addr,sizeof(client.sin_addr.s_addr),client.sin_family)) == NULL)
		{
			printf("Error with gethostbyaddr\n");
			exit(1);
		}
		printf("From %s, address %s - \n",rem->h_name,inet_ntoa(client.sin_addr));

		/* Create thread for serving client */
		to_pass.new_sock = newsock;
		to_pass.ip_address = malloc(sizeof(char)*strlen(inet_ntoa(client.sin_addr)) + sizeof(char));
		if (to_pass.ip_address == NULL) { printf("Error with malloc\n"); exit(1); }
		strcpy(to_pass.ip_address,inet_ntoa(client.sin_addr));
		if (pthread_create( &thr, NULL, child_server_thread, &to_pass) != 0)
		{
			printf("Error with pthtread_create\n");
			exit(1);
		}

		sleep(1);
		free(to_pass.ip_address);
	}

	return 0;
}
