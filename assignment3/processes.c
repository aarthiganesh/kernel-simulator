#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <errno.h>

#include "message.h"

//defining the queue ids
#define ATMQUEUE 12345
#define DBSERVERQUEUE 11111
#define MAX_TEXT_SIZE 500

int main (){
  pid_t pid;
  char *message;
  int n;

  printf("enter value: ");
  scanf("%i",&n);

  printf("fork program starting\n");
  pid = fork();

  while (1){

    if(pid!=0){//parent
      if (n==1){
        printf("PARENT\n");
        printf("enter value: ");
        scanf("%i",&n);
        printf("%i",n);
      }
    }else{
      if (n==2){
          printf("CHILD\n");
          system("gcc process2.c -o p2");
          execl("./p2","p2.",NULL);
        }
    }
    
    // switch(pid)
    // {
    //   case -1:
    //     perror("fork failed\n");
    //     exit(1);
    //   case 0: // CHILD
    //     if (n==2){
    //       printf("HERE\n");
    //       system("gcc process2.c -o p2");
    //       execl("./p2","p2.",NULL);
    //     }
    //     break;
    //   default: // PARENT
    //     if (n==1){
    //       printf("PARENT\n");
    //       printf("enter value: ");
    //       scanf("%i",&n);
    //     }
    //     break;
    // }
  }

  exit(0);
}