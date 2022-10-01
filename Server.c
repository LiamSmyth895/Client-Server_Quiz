#include "QuizDB.h"
#include "Quiz.h"

char *intro = "Welcome to the Unix Programming quiz!\nThe quiz comprises five questions posed to you one after the other.\nYou have only one attempt to answer a question.\nYour final score will be sent to you after conclusion of the quiz.\nTo start the quiz, press Y and <enter>.\nTo quit the quiz, press q and <enter>.";

int main(int argc, char **argv){

  if (argc != 3)
  {
     fprintf(stderr, "Usage: %s <IPv4 Address> <port number>.\n",
             argv[0]);
     exit(-1);
  }

  struct sockaddr_in serverAddress;

  memset(&serverAddress, 0, sizeof(struct sockaddr_in));

  serverAddress.sin_family = AF_INET;
  serverAddress.sin_addr.s_addr = inet_addr(argv[1]);
  serverAddress.sin_port = htons(atoi(argv[2]));

  int lfd = socket(AF_INET, SOCK_STREAM, 0);
  if (lfd == -1) {
     fprintf(stderr, "socket() error.\n");
     exit(-1);
  }

  // set the sock option the server to reuse the server endpoint immediately after its terminated
  int optval = 1;
  if (setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1){
      exit(-1);
  }

  int rc = bind(lfd, (struct sockaddr *)&serverAddress, sizeof(struct sockaddr));
  if (rc == -1) {
      fprintf(stderr, "bind() error.\n");
      exit(-1);
  }

  if (listen(lfd, BACKLOG) == -1){
    exit(-1);
  }

  fprintf(stdout, "<Listening on %s:%s>\n", argv[1], argv[2]);
  fprintf(stdout, "<ctrl-C to terminate>\n");

  for (;;) /* Handle clients iteratively */
  {

      struct sockaddr_storage claddr;
      socklen_t addrlen = sizeof(struct sockaddr_storage);
      int cfd = accept(lfd, (struct sockaddr *)&claddr, &addrlen);
      if (cfd == -1) {
        // no client is trying to connect yet
         continue;
      }
      printf("%s\n", "client has connected");

      {
         char host[NI_MAXHOST];
         char service[NI_MAXSERV];
         if (getnameinfo((struct sockaddr *) &claddr, addrlen,
                   host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
            fprintf(stdout, "Connection from (%s, %s)\n", host, service);
         else
            fprintf(stderr, "Connection from (?UNKNOWN?)");
      }

      char clientInput[BUFSIZE];

      sendToConnection(intro, cfd); // send the quiz introduction to the client

      recieveFromConnection(clientInput, cfd); // let the client say of they want to playe the quiz or not

      if(strcmp(clientInput, "Y\0") == 0 || strcmp(clientInput, "y\0") == 0){
        // play the quiz
        int questionsAsked[] = {-1,-1,-1,-1,-1}; // array to keep track of what questions have been asked so far
        int qIndex; // used to store the index of the current question being asked
        int numq = sizeof(QuizQ)/sizeof(QuizQ[0]);
        int score = 0;
        srand(time(0));

        for(int i = 0; i < 5; i++){
          // main quiz loop
          qIndex = rand() % numq;
          bool usedQ = false;

          for(int j = 0; j < 5 && !usedQ; j++){
            // check if this question has already been asked
            if(questionsAsked[j] == qIndex){
              usedQ = true;
            }
          }
          if(usedQ){
            // if this question has already been asked, run this iteration again
            i--;
            continue;
          }
          questionsAsked[i] = qIndex;

          sendToConnection(QuizQ[qIndex], cfd);
          recieveFromConnection(clientInput, cfd);

          // tell the client if they got the answer right or not
          if(strcmp(clientInput, QuizA[qIndex]) == 0){
            sendToConnection("Right Answer", cfd);
            score++;
          }else{
            char *wrongAnswerPrompt = "Wrong answer, The right answer is ";
            char *fullPrompt = concatenate(wrongAnswerPrompt, QuizA[qIndex]);
            sendToConnection(fullPrompt, cfd);
          }
        }

        // tell the client what their score in the quiz was
        char *quizEnding1 = "Your quiz score is ";
        char *quizEnding2 = "/5. Goodbye!";
        char *stringScore;
        asprintf(&stringScore, "%d", score); // turn the score from an integer to a string
        char *endingPromptStart = concatenate(quizEnding1, stringScore);
        char *fullEndingPrompt = concatenate(endingPromptStart, quizEnding2);

        sendToConnection(fullEndingPrompt, cfd);

      }else{
        // client did not enter Y
        sendToConnection("Closing connection\n", cfd);
      }


      if (close(cfd) == -1) /* Close connection */
      {
         fprintf(stderr, "close error.\n");
         exit(EXIT_FAILURE);
      }
      printf("%s\n", "Client has been served, waiting for next connection");
  }

  if (close(lfd) == -1) /* Close listening socket */
  {
     fprintf(stderr, "close error.\n");
     exit(EXIT_FAILURE);
  }

  exit(EXIT_SUCCESS);

  return 0;
}
