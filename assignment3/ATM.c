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
#define DBSERVERQUEUE 54321
#define MAX_TEXT_SIZE 500

char inputpin[3];
char inputaccount[5];
char bankingoption[20];
int counter = 0;
int inputvalid = 0;

struct message dataToSend;
struct message dataReceived;

void requestinput(void){
  // REQUEST INPUT DATA FROM ATM USER
  printf("Enter pin number: ");
	scanf("%s", inputpin);
	printf("Enter account number: ");
	scanf("%s", inputaccount);

  //READ PIN NUMBER
	strcpy(dataToSend.pin,inputpin);
	//READ ACCOUNT NUMBER
	strcpy(dataToSend.accountnumber,inputaccount);
}

int main(){
	int msgidATM;
	int msgidServer;

	msgidATM = msgget((key_t)ATMQUEUE, 0666|IPC_CREAT);
	msgidServer = msgget((key_t)DBSERVERQUEUE, 0666|IPC_CREAT);

	while(1){
		requestinput();
		inputvalid = 1;
		strcpy(dataToSend.text,"UPDATE");
			if(inputvalid){
				printf("Data sent from ATM to DBSERVER is : %s, %s, %s\n", dataToSend.pin, dataToSend.accountnumber, dataToSend.text);
				if(msgsnd(msgidATM, &dataToSend, sizeof(struct message),0) == -1){
					printf("Error msgsnd failed\n");
				}
				inputvalid = 0;
			}
	}

}