#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "message.h"

#define DBEDITORQUEUE 54321

struct message dataToSend;

char inputpin[3];
char inputaccount[5];
float funds;

int main()
{
	int msgidEditor;
	int running = 1;
	int update;
	msgidEditor = msgget((key_t)DBEDITORQUEUE, 0666|IPC_CREAT);
	

	printf("DBEDITOR QUEUE: %d\n",msgidEditor);
	
	while(running){
		printf("Enter pin number: ");
		scanf("%s", inputpin);
		printf("Enter account number: ");
		scanf("%s", inputaccount);
		printf("Enter funds available: ");
		scanf("%f",&funds);
		update = 1;

		if(update){

			//pin
			strcpy(dataToSend.pin,inputpin);

			//account
			strcpy(dataToSend.accountnumber,inputaccount);

			//funds		
			dataToSend.balance = funds;
		
			strcpy(dataToSend.text,"UPDATE_DB");

			if(msgsnd(msgidEditor, &dataToSend, sizeof(struct message),0) == -1){
				printf("Error msgsnd failed\n");
			}
			printf("pin is: %s\n",dataToSend.pin);
			printf("Data sent from dbeditor to dbserver is : %s, %s,%f, %s\n", dataToSend.pin, dataToSend.accountnumber,dataToSend.balance, dataToSend.text);
			update = 0;
		}
		
	}
}
