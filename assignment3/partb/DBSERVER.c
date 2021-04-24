#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/sem.h>

#include "semaphores.h"
#include "message.h"
#include "dbarray.h"
#include "semun.h"

//defining the queue ids
#define ATMQUEUE 12345
#define DBSERVERQUEUE 11111
#define DBEDITORQUEUE 12346
#define MAX_TEXT_SIZE 20

//file altering variables
FILE *dbfile = NULL;
char line [128][100];
int numAccounts = 0;

void printOption(struct message dataReceived);
void readDB(struct dbarray dbArray[], int numAccounts);
void updateDB(struct dbarray dbArray[], int numAccounts);
void printDB(struct dbarray dbArray[], int numAccounts);
void calcNumAcct();
char blockAccount (char account [5]);

// message structs
struct message dataToSend;
struct message dataReceived;
struct message dataToUpdate;

// semaphore variables
static int sem_id;



int main()
{
	int msgidATM;
	int msgidServer;
	int msgidEditor; // DBSERVER <--> DBEDITOR
	int actExists;
	int recipient;
	int recipientid;
	int pinBlock [100] = {0};//limit of 100 accounts
	char acctnum [6];

	printf("Hello, DBSERVER\n");

	//this queue will be used to send data from atm to dbserver, dbserver will receive
	msgidATM = msgget((key_t)ATMQUEUE, 0666|IPC_CREAT);
	msgidServer = msgget((key_t)DBSERVERQUEUE, 0666|IPC_CREAT);
	msgidEditor = msgget((key_t)DBEDITORQUEUE, 0666|IPC_CREAT);

	printf("ATM QID: %d\n",msgidATM);
	printf("DBSERVER QID: %d\n",msgidServer);
	printf("DBEDITOR QID: %d\n",msgidEditor);
	
	//to receive the first message
	while(1){

		//RECEIVE MESSAGE FROM ATM
		if(msgrcv(msgidATM, &dataReceived, sizeof(struct message),0,IPC_NOWAIT)!= -1){
		// 	// printf("Error msgrcv failed");	
		// }else{
			printf("\nPIN and ACCT received\n");
			actExists = 0;
			// read the db
			calcNumAcct();
			struct dbarray dbArray[numAccounts];
			readDB(dbArray, numAccounts);
			// printDB(dbArray, numAccounts);

			for(int i=0;i<numAccounts;i++){
				if(strcmp(dataReceived.accountnumber,dbArray[i].accountnumber) == 0){
					strcpy(dataToSend.accountnumber,dbArray[i].accountnumber);
					actExists = 1;
					//pin validation
					if(strcmp(dataReceived.pin,dbArray[i].pin) == 0){
						printf("PIN_CORRECT\n");
						strcpy(dataToSend.text,"OK");
						if(msgsnd(msgidServer, &dataToSend, sizeof(struct message),0) == -1){
							printf("ERROR: OK message not sent\n");
						}
						
						if(msgrcv(msgidATM, &dataReceived, sizeof(struct message),0,0)== -1){
							printf("Error msgrcv failed");	
						}else{
							printOption(dataReceived);
						}

						// RETURN BALANCE
						if(strcmp(dataReceived.text,"balance")==0 ){
							printf("sending balance ...\n");
							dataToSend.balance = dbArray[i].balance;
						
						// WITHDRAWAL
						}else if (strcmp(dataReceived.text,"withdraw")==0 ){
							printf("WITHDRAWAL ATTEMPT: ");
							if(dataReceived.withdrawal<=dbArray[i].balance & dbArray[i].balance>0){
								printf("SUCCESSFUL\n");
								strcpy(dataToSend.text,"FUNDS_OK");
								dbArray[i].balance = dbArray[i].balance - dataReceived.withdrawal;
								dataToSend.balance = dbArray[i].balance;
								updateDB(dbArray,numAccounts);
							}else{
								dataToSend.balance = dbArray[i].balance;
								strcpy(dataToSend.text,"NSF");
								printf("UNSUCCESSFUL, NSF\n");
							}

						// MONEY TRANSFER
						// }else if (strcmp(dataReceived.text,"transfer")==0){
						// 	printf("TRANSFER ATTEMPT: ");
						// 	if(dataReceived.withdrawal<=dbArray[i].balance & dbArray[i].balance>0){
						// 		for(int j=0;j<numAccounts;j++){
						// 			if(strcmp(dbArray[j].accountnumber,dataReceived.accountnumber)){
						// 				recipientid=j+1;
						// 			}
						// 		}
						// 		if(recipientid>0){
						// 			printf("SUCCESSFUL\n");
						// 			strcpy(dataToSend.text,"FUNDS_OK");
						// 			dbArray[i].balance = dbArray[i].balance - dataReceived.withdrawal;
						// 			recipientid = recipientid-1;
						// 			dbArray[recipientid].balance = dbArray[recipientid].balance + dataReceived.withdrawal;
						// 			dataToSend.balance = dbArray[i].balance;
						// 			updateDB(dbArray,numAccounts);
						// 			recipientid=0;
						// 		}else{
						// 			printf("Recipient does not exist.");
						// 			strcpy(dataToSend.text,"DNE");
						// 		}
						// 	}else{
						// 		dataToSend.balance = dbArray[i].balance;
						// 		strcpy(dataToSend.text,"NSF");
						// 		printf("UNSUCCESSFUL, NSF\n");
						// 	}
						}else{
							printf("UNRECOGNIZED BANKING OPTION\n");
							strcpy(dataToSend.text,"UNKNOWN OPTION");
						}
						// send response
						if(msgsnd(msgidServer, &dataToSend, sizeof(struct message),0) == -1){
							printf("ERROR: OK message not sent\n");
						}

					}else{
						printf("PIN_ERROR, %i\n",dbArray[i].count);
						//account locked if pin wrong 3 times
						dbArray[i].count ++;
						if(dbArray[i].count==3){
							strcpy(dbArray[i].accountnumber,"X");
							printf("ACCOUNT LOCKED\n");
							printf("%s\n",dbArray[i].accountnumber);
						}
						strcpy(dataToSend.text,"PIN_ERROR");

						pinBlock[i] ++;

						if(pinBlock[i]==3){
							strcpy(&dbArray[i].accountnumber[4],"X");
							updateDB(dbArray,numAccounts);
						}
		
						if(msgsnd(msgidServer, &dataToSend, sizeof(struct message),0) == -1){
							printf("ERROR: OK message not sent\n");
						}
					}
				}
			}
			if (actExists==0){
				strcpy(dataToSend.text,"DNE");
				if(msgsnd(msgidServer, &dataToSend, sizeof(struct message),0) == -1){
					printf("ERROR: OK message not sent\n");
				}
			}
			numAccounts = 0;
		}
		if(msgrcv(msgidEditor, &dataReceived, sizeof(struct message),0,IPC_NOWAIT)!= -1){
			printf("\nUPDATED DB\n");
		}
	}
}

void printOption(struct message dataReceived){
	printf("BANKING OPTION: %s\n",dataReceived.text);
}

void calcNumAcct(){
	dbfile = fopen("db.txt", "r");

	while(fgets(line[numAccounts], 200, dbfile)) 
	{
		numAccounts++;
	}
	fclose(dbfile);
}

void readDB(struct dbarray dbArray[], int numAccounts){
	
	dbfile = fopen("db.txt", "r");
	
	printf("\nREADING DB FILE CONTENT\n");
	printf("Number of Accounts: %i\n",numAccounts);
	for(int i=0;i<numAccounts;i++)
	{
			sscanf(line[i] , "%s %s %f\n", dbArray[i].accountnumber, dbArray[i].pin, &dbArray[i].balance);
			dbArray[i].count=0;
	}

	fclose(dbfile);
	printf("\n");
}

void updateDB(struct dbarray dbArray[], int numAccounts){

	dbfile = fopen("db.txt", "w");
	// printf("Number of Accounts: %i\n",numAccounts);
	//Print contents of collection into the file with proper formatting
	for (int i = 0; i < numAccounts; i++) {
		fprintf(dbfile, "%s %s %.2f\n", dbArray[i].accountnumber, dbArray[i].pin, dbArray[i].balance);
	}
	//Close file and return
	fclose(dbfile);
}

void printDB(struct dbarray dbArray[], int numAccounts){
	dbfile = fopen("db.txt", "r");

	printf("\nCURRENT FILE CONTENTS\n");
	printf("Number of Accounts: %i\n",numAccounts);
	for(int i=0;i<numAccounts;i++)
	{
			printf("Acct#:%s pin:%s balance: $%.2f\n", dbArray[i].accountnumber, dbArray[i].pin, dbArray[i].balance);//DEBUG
	}

	fclose(dbfile);
	printf("END\n\n");
}
