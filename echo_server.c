/******************************************************************************
* echo_server.c                                                               *
*                                                                             *
* Description: This file contains the C source code for an echo server.  The  *
*              server runs on a hard-coded port and simply write back anything*
*              sent to it by connected clients.  It does not support          *
*              concurrent clients.                                            *
*                                                                             *
* Authors: Athula Balachandran <abalacha@cs.cmu.edu>,                         *
*          Wolf Richter <wolf@cs.cmu.edu>                                     *
*                                                                             *
*******************************************************************************/
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>

#define ECHO_PORT 9999
#define BUF_SIZE 4096
#define LOG_FILE "log.txt"
#define MAX_CONN 1250


/************** Global variables **************/
int client_sockets[MAX_CONN];		/* Array to track all connections */
int max_sd;				/* Max socket descriptor */
fd_set readfds;				/* Read file descriptors for incoming conn */

void log_write(char* msg){
    FILE* file_ptr = fopen(LOG_FILE, "a+");

    if(file_ptr != NULL){
	fputs(msg, file_ptr);
	fclose(file_ptr);
    }
}

int close_socket(int sock)
{
    if (close(sock))
    {
        fprintf(stderr, "Failed closing socket.\n");
        return 1;
    }
    return 0;
}

int main(int argc, char* argv[])
{
    int sock, new_socket;
    int i, count;
    int sd, selected_fd;
    ssize_t readret;
    socklen_t cli_size;
    struct sockaddr_in addr, cli_addr;
    char buf[BUF_SIZE];

    /* Initialize client connections */
    for(i=0; i<MAX_CONN; i++){
	client_sockets[i] = 0;
    }

    fprintf(stdout, "----- Echo Server -----\n");
    char* msg = "***** Starting server *****";
    log_write(msg);

    /* Primary server socket - creates end point for binding */
    if ((sock = socket(PF_INET, SOCK_STREAM, 0)) == -1)
    {
        fprintf(stderr, "Failed creating socket.\n");
        return EXIT_FAILURE;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(ECHO_PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    /* servers bind sockets to ports---notify the OS they accept connections */
    if (bind(sock, (struct sockaddr *) &addr, sizeof(addr))){
        close_socket(sock);
        fprintf(stderr, "Failed binding socket.\n");
        return EXIT_FAILURE;
    }


    /* Server listen for incoming connections */
    if (listen(sock, 5)){
        close_socket(sock);
        fprintf(stderr, "Error listening on socket.\n");
        return EXIT_FAILURE;
    }

    /*********************** Server Core **********************/
    /* finally, loop waiting for input and then write it back */
    while(1){
       /* clear the socket set */
       FD_ZERO(&readfds);

       /* add master socket to set */
       FD_SET(sock, &readfds);
       max_sd = sock;

	count = 0;
       /* add Provision for MAX_CONN child sockets */
       for(i=0; i<MAX_CONN; i++) {
            // socket descriptor
            sd = client_sockets[i];

            // if socket descriptor is valid then add to read list
            if(sd > 0){
		count++;
                FD_SET(sd , &readfds);
	    }

            // save highest file descriptor number, for use in select function
            if(sd > max_sd)
                max_sd = sd;
        }

	//printf("Max Socket FD in use: %d \n", max_sd);
	printf("Active connections: %d \n", count);

	/* wait for select to return, timeout is NULL - so wait indefinitely */
	selected_fd = select( max_sd + 1 , &readfds , NULL , NULL , NULL);

	if(selected_fd < 0){
	    msg = "Select() error \n";
	    printf("%s", msg);
	    log_write(msg);
	}

	cli_size = sizeof(cli_addr);
        // If master socket is set, then there is a new incoming connection
        if(FD_ISSET(sock, &readfds)){
            if((new_socket = accept(sock, (struct sockaddr *)&cli_addr, (socklen_t*)&cli_size))<0){
                printf("Failed to accept incoming connection!");
                exit(EXIT_FAILURE);
            }

	    // inform user of socket number - used in send and receive command
	    printf("New connection::  socket fd=%d , ip: %s , port: %d \n", new_socket,
		inet_ntoa(cli_addr.sin_addr) , ntohs(cli_addr.sin_port));

	/*
	    msg = "Simple Web server version 1.0 \n";
	    // send new connection greeting message
	    if( send(new_socket, msg, strlen(msg), 0) != strlen(msg) ){
		printf("send error");
	    }

	    puts("Welcome message sent!");
	*/

            // add new socket to the first free position in client sockets array
	    for (i = 0; i < MAX_CONN; i++){
		// if position is empty
		if( client_sockets[i] == 0 ){
                    client_sockets[i] = new_socket;
                    printf("Adding to list of client_sockets at %d\n" , i);
                    break;
                }
            }
        }

	/* Handle all concurrent connections in the client array */
        for(i = 0; i < MAX_CONN; i++){
	    sd = client_sockets[i];

            if(FD_ISSET(sd, &readfds)){
                // Check if it was for closing, and also read the incoming message
                if ((readret = read(sd, buf, BUF_SIZE)) <= 0){
                    // Somebody disconnected, get his details and print
                    getpeername(sd , (struct sockaddr*)&cli_addr , (socklen_t*)&cli_size);
                    printf("Host disconnected:: ip %s , port %d \n",
			   inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));

                    // Close the socket and mark as 0 in list for reuse
                    printf("Closing connection: ip %s , port %d \n",
                           inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));
                    close(sd);
                    client_sockets[i] = 0;
                }else{
		    printf("Reading client message size: %ld  \n", readret);
                    // set the string terminating NULL byte on the end of the data read
                    buf[readret] = '\0';
                    send(sd , buf , strlen(buf) , 0);
                }

		// Clear buffer after message was sent back to client
		memset(buf, 0, BUF_SIZE);
            }
        }


       /* ----------- OLD CODE -----------*/
       /*
       cli_size = sizeof(cli_addr);
       if ((client_sock = accept(sock, (struct sockaddr *) &cli_addr,
                                 &cli_size)) == -1)
       {
           close(sock);
           fprintf(stderr, "Error accepting connection.\n");
           return EXIT_FAILURE;
       }else{
	   printf("Client connected \n");
       }

       readret = 0;

       while((readret = recv(client_sock, buf, BUF_SIZE, 0)) >= 1)
       {
	   printf("Received: %s", buf );
           if (send(client_sock, buf, readret, 0) != readret)
           {
               close_socket(client_sock);
               close_socket(sock);
               fprintf(stderr, "Error sending to client.\n");
               return EXIT_FAILURE;
           }
           memset(buf, 0, BUF_SIZE);
       } 

       if (readret == -1)
       {
           close_socket(client_sock);
           close_socket(sock);
           fprintf(stderr, "Error reading from client socket.\n");
           return EXIT_FAILURE;
       }

       if (close_socket(client_sock))
       {
           close_socket(sock);
           fprintf(stderr, "Error closing client socket.\n");
           return EXIT_FAILURE;
       }

    }

    close_socket(sock);

    return EXIT_SUCCESS;
  */
    }

    close_socket(sock);

    return EXIT_SUCCESS;
}