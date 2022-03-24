#ifndef USERSOCKET_H
#define USERSOCKET_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <error.h>
#include <time.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <pthread.h>

#define BUF_SIZE    (9*1024)

int server_create(int no_port, unsigned int backLog);
int client_create(int no_port, char* ip);

void * doNetworking(void * ClientDetail);
void * doRecieving(void * ClientDetail);

int socketSend(int sockfd, char* data, int len);
int socketClose(int sockfd, int how);

#endif // SOCKET_H
