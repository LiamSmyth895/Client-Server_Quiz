#include "Quiz.h"

int main(int argc, char *argv[]){
  if (argc != 3){
      fprintf(stderr, "Usage: %s <IP address of server> <port number>.\n",
             argv[0]);
      exit(-1);
  }

  struct sockaddr_in serverAddress;

  memset(&serverAddress, 0, sizeof(struct sockaddr_in));
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_addr.s_addr = inet_addr(argv[1]);
  serverAddress.sin_port = htons(atoi(argv[2]));

  int cfd = socket(AF_INET, SOCK_STREAM, 0);
  if (cfd == -1) {
     fprintf(stderr, "socket() error.\n");
     exit(-1);
  }

  int rc = connect(cfd, (struct sockaddr *)&serverAddress,
                   sizeof(struct sockaddr));
  if (rc == -1) {
     fprintf(stderr, "connect() error, errno %d.\n", errno);
     exit(-1);
  }

  char receivedMessage[BUFSIZE];
  char quizStart[1];

  recieveFromConnection(receivedMessage, cfd); // receive intro from the server
  printf("%s\n", receivedMessage);

  scanf("%s", quizStart);
  sendToConnection(quizStart, cfd);

  if(strcmp(quizStart, "Y\0") == 0 || strcmp(quizStart, "y\0") == 0){
    // begin the quiz if Y or y is entered
    printf("%s\n", "starting the quiz");
    for(int i = 0; i < 5; i++){
      recieveFromConnection(receivedMessage, cfd); // receive question from server
      printf("%s\n", receivedMessage);// print out the qustion for the user
      char answer[BUFSIZE];
      scanf("%s", answer);
      sendToConnection(answer, cfd); // send answer to server
      recieveFromConnection(receivedMessage, cfd);
      printf("%s\n", receivedMessage);
    }

    recieveFromConnection(receivedMessage, cfd); // receive your score from the server
    printf("%s\n", receivedMessage);
  }

  recieveFromConnection(receivedMessage, cfd); // let the user know that the connection is being closed
  printf("%s\n", receivedMessage);

  if (close(cfd) == -1){ /* Close connection */
    fprintf(stderr, "close error.\n");
    exit(EXIT_FAILURE);
  }

  exit(EXIT_SUCCESS);



  return 0;
}
