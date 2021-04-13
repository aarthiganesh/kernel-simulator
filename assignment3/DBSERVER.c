#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "message.h"
#include "dbarray.h"

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
	char line [128][10];
	int numAccounts;
	int msgidATM;
	int msgidServer;
	int msgidEditor; // DBSERVER <--> DBEDITOR
	int acctCounter = 0;

	//Data structs to send and receive from message queue
	printf("DB File Open\n");
  dbfile = fopen("db.txt", "r+"); //create and open output.txt file to writereate and open output.txt file to write

	while(fgets(line[acctCounter], 200, dbfile)) 
	{
			line[acctCounter][strlen(line[acctCounter]) - 1] = '\0';
			acctCounter++;
	}
    
  numAccounts = acctCounter;
	struct dbarray dbArray[numAccounts];

	for(int i=0;i<numAccounts;i++)
	{
			sscanf(line[i] , "%s %s %f", dbArray[i].accountnumber, dbArray[i].pin, &dbArray[i].balance);
			// printf("Acct#:%s pin:%s balance: $%.2f", dbArray[i].accountnumber, dbArray[i].pin, dbArray[i].balance);//DEBUG
	}

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

		//RECEIVE MESSAGE FROM ATM
		if(msgrcv(msgidATM, &dataReceived, sizeof(struct message),0,0)== -1){
			printf("Error msgrcv failed");	
		}else{
			for(int i=0;i<numAccounts;i++){
				if(strcmp(dataReceived.accountnumber,dbArray[i].accountnumber) == 0){
					printf("YAAYYY ACCOUNT EXISTS");
				}
			}
		}

		// DATA FROM ATM
		printf("Data Received from ATM is : %s %s\n",dataReceived.pin,dataReceived.accountnumber);

		

		// //RECEIVE MESSAGE FROM EDITOR
		// if(msgrcv(msgidEditor, &dataToUpdate, sizeof(struct message),0,0)== -1){
		// 	printf("Error msgrcv failed");	
		// }
		// // DATA FROM DBEDITOR
		// printf("Data Received from DBEDITOR is : %s %s %f %s\n",dataToUpdate.pin,dataToUpdate.accountnumber,dataToUpdate.balance,dataToUpdate.text);

		// if(strcmp(dataToUpdate.text,"UPDATE_DB") == 0){
		// 	printf("DATABASE HAS BEEN UPDATED WITH:\n\tPIN: %s\n\tAccount Number: %s\n\tBalance:$%.2f\n\t%s\n",dataToUpdate.pin,dataToUpdate.accountnumber,dataToUpdate.balance,dataToUpdate.text);

		// 	fprintf(dbfile,"%s\t%s\t%f\n",
		// 		dataToUpdate.pin,
		// 		dataToUpdate.accountnumber,
		// 		dataToUpdate.balance);
		// 	//add 1 to pin number to encrypt
		// }
	}
	

}