#include "Quiz.h"

void recieveFromConnection(char *input, int cfd){
  size_t totRead;
  char* bufr = input;
  for(totRead = 0; totRead < BUFSIZE; ){
    ssize_t numRead = read(cfd, bufr, BUFSIZE - totRead);
    if(numRead == 0){
      break;
    }
    if(numRead == -1){
      if(errno == EINTR){
        continue;
      }else{
        fprintf(stderr, "Read error.\n");
      }
    }
    totRead += numRead;
    bufr += numRead;
  }
  input[totRead - 1] = '\0';
}

void sendToConnection(char *output, int cfd){
  size_t totWritten;
  const char* bufw = output;
  for(totWritten = 0; totWritten < BUFSIZE; ){
    ssize_t numWritten = write(cfd, bufw, BUFSIZE - totWritten);
    if(numWritten <= 0){
      if(numWritten == -1 && errno == EINTR){
        continue;
      }else{
        fprintf(stderr, "Write error\n");
        exit(EXIT_FAILURE);
      }
    }
    totWritten += numWritten;
    bufw += numWritten;
  }
}

char* concatenate(const char *str1, const char *str2){
  char *result = malloc(strlen(str1) + strlen(str2) + 1);
  if(result==NULL)
  {
    printf("Error, malloc() failure.");
    exit(0);
  }
  strcpy(result, str1);
  strcat(result, str2);
  return result;
}
