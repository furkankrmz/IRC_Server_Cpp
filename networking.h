#ifndef NETWORKING_H
#define NETWORKING_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdlib>
#include <iostream>
#include <unistd.h>

// Functions to create a socket, bind the socket to a port, listen for connections, and accept a connection.
int createSocket();
void bindSocket(int sockfd, int portNumber);
void startListening(int sockfd);
int acceptConnection(int sockfd);

#endif // NETWORKING_H
