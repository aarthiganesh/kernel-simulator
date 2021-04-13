#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "message.h"

//defining the queue ids
#define ATMQUEUE 12345
#define DBSERVERQUEUE 54321
#define DBEDITORQUEUE 54321
#define MAX_TEXT_SIZE 20

FILE *dbfile = NULL;

struct message dataToSend;
struct message dataReceived;
struct message dataToUpdate;

int main()
{
	int msgidATM;
	int msgidServer;
	int msgidEditor; // DBSERVER <--> DBEDITOR

	//Data structs to send and receive from message queue
	printf("DB File Open\n");
  dbfile = fopen("db.txt", "r+"); //create and open output.txt file to writereate and open output.txt file to write

	printf("Hello, DBSERVER\n");

	//this queue will be used to send data from atm to dbserver, dbserver will receive
	msgidATM = msgget((key_t)ATMQUEUE, 0666|IPC_CREAT);
	msgidServer = msgget((key_t)DBSERVERQUEUE, 0666|IPC_CREAT);
	msgidEditor = msgget((key_t)DBEDITORQUEUE, 0666|IPC_CREAT);

	printf("ATM QUEUE: %d\n",msgidATM);
	printf("DBSERVER QUEUE: %d\n",msgidServer);
	printf("DBEDITOR QUEUE: %d\n",msgidEditor);
	
	//to receive the first message
	while(1){
		//RECEIVE MESSAGE FROM EDITOR
		if(msgrcv(msgidEditor, &dataToUpdate, sizeof(struct message),0,0)== -1){
			printf("Error msgrcv failed");	
		}
		// DATA FROM DBEDITOR
		printf("Data Received is : %s %s %f %s\n",dataToUpdate.pin,dataToUpdate.accountnumber,dataToUpdate.balance,dataToUpdate.text);

		if(strcmp(dataToUpdate.text,"UPDATE_DB") == 0){
			printf("DATABASE HAS BEEN UPDATED WITH:\n\tPIN: %s\n\tAccount Number: %s\n\tBalance:$%.2f\n\t%s\n",dataToUpdate.pin,dataToUpdate.accountnumber,dataToUpdate.balance,dataToUpdate.text);

			fprintf(dbfile,"%s\t%s\t%f\n",
				dataToUpdate.pin,
				dataToUpdate.accountnumber,
				dataToUpdate.balance);
			//add 1 to pin number to encrypt
		}
	}
	

}