#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <errno.h>
#include <ctype.h>

#define BACKLOG 10
#define BUFSIZE 512

void recieveFromConnection(char *input, int cfd);
void sendToConnection(char *output, int cfd);
char* concatenate(const char *str1, const char *str2);
