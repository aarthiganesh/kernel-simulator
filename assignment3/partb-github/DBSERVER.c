
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/shm.h>
#include "message.h"
#include "dbarray.h"

//defining the queue ids
#define ATMQUEUE 12345
#define DBSERVERQUEUE 11111
#define DBEDITORQUEUE 54321
#define MAX_TEXT_SIZE 20

FILE *dbfile = NULL;

struct message dataToSend;
struct message dataReceived;
struct message dataToUpdate;

// void update_db(struct dbarray updated_dbArray, int length){
// 	if(msgsnd(msgidServer, &dataToSend, sizeof(struct message),0) == -1){
// 		printf("ERROR: OK message not sent\n");
// 	}
// }


int main()
{
	char line [128][10];
	int numAccounts;
	int msgidATM;
	int msgidServer;
	int msgidEditor; // DBSERVER <--> DBEDITOR
	int acctCounter = 0;
	int shmid;
	char (*variable)[20];
	key_t key = ftok("shmfile",65);

	shmid = shmget(key, 1024, 0644|IPC_CREAT);
	if (shmid == -1) {
		perror("Shared memory");
		return 1;
	}
	//Data structs to send and receive from message queue
	printf("DB File Open\n");
	dbfile = fopen("db.txt", "r+"); //create and open output.txt file to writereate and open output.txt file to write

	while(fgets(line[acctCounter], 200, dbfile)) 
	{
			acctCounter++;
	}
    
  numAccounts = acctCounter;
	struct dbarray dbArray[numAccounts];

	for(int i=0;i<numAccounts;i++)
	{
			sscanf(line[i] , "%s %s %f", dbArray[i].accountnumber, dbArray[i].pin, &dbArray[i].balance);
			dbArray[i].count=0;
			printf("Acct#:%s pin:%s balance: $%.2f\n", dbArray[i].accountnumber, dbArray[i].pin, dbArray[i].balance);//DEBUG
	}

	printf("Hello, DBSERVER\n");

	//this queue will be used to send data from atm to dbserver, dbserver will receive
	msgidATM = msgget((key_t)ATMQUEUE, 0666|IPC_CREAT);
	msgidServer = msgget((key_t)DBSERVERQUEUE, 0666|IPC_CREAT);
	// msgidEditor = msgget((key_t)DBEDITORQUEUE, 0666|IPC_CREAT);

	printf("ATM QUEUE: %d\n",msgidATM);
	printf("DBSERVER QUEUE: %d\n",msgidServer);
	printf("DBEDITOR QUEUE: %d\n",msgidEditor);
	
	//to receive the first message
	while(1){

		// msg_rcv(msgidATM,0);

		//RECEIVE MESSAGE FROM ATM
		if(msgrcv(msgidATM, &dataReceived, sizeof(struct message),0,0)== -1){
			// printf("Error msgrcv failed");	
		}else{
			printf("----------------PIN and ACCT received-------------------\n");
			for(int i=0;i<numAccounts;i++){
				if(strcmp(dataReceived.accountnumber,dbArray[i].accountnumber) == 0){
					strcpy(dataToSend.accountnumber,dbArray[i].accountnumber);
					//pin validation
					if(strcmp(dataReceived.pin,dbArray[i].pin) == 0){
						printf("PIN_CORRECT\n");
						strcpy(dataToSend.text,"OK");
						if(msgsnd(msgidServer, &dataToSend, sizeof(struct message),0) == -1){
							printf("ERROR: OK message not sent\n");
						}

						if(msgrcv(msgidATM, &dataReceived, sizeof(struct message),0,0)== -1){
							printf("Error msgrcv failed");	
						}
						printf("%s\n",dataReceived.text);

						if(strcmp(dataReceived.text,"balance")==0 ){
							printf("balance sending ...\n");
							dataToSend.balance = dbArray[i].balance;
						}else if (strcmp(dataReceived.text,"withdraw")==0 ){
							printf("WITHDRAW AMOUNT\n");
							if(dataReceived.withdrawal<=dbArray[i].balance){
								strcpy(dataToSend.text,"FUNDS_OK");
								dbArray[i].balance = dbArray[i].balance - dataReceived.withdrawal;
								dataToSend.balance = dbArray[i].balance;
							}else{
								strcpy(dataToSend.text,"NSF");
							}
						//pay bill: verify if there is enough balance and pays the amount
						}else if(strcmp(dataReceived.text,"paybill")==0 ){
							printf("PROCESSING BILL\n");
							if(dataReceived.bill<=dbArray[i].balance){
								//strcpy(dataToSend.text,"FUNDS_OK");
								dbArray[i].balance = dbArray[i].balance - dataReceived.bill;
								dataToSend.balance = dbArray[i].balance;

								variable = shmat(shmid,(void*)0,0);
								printf("Received response for receipt: %s\n",variable);
								if(strcmp(variable[0],"y")== 0){
									printf("========RECEIPT=========\n");
									printf("Bill Payment of $ %f was made\n",dataReceived.bill);
									printf("Balance Remaining is $ %f\n", dbArray[i].balance);
									printf("===========================\n");
								}
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
						if(msgsnd(msgidServer, &dataToSend, sizeof(struct message),0) == -1){
							printf("ERROR: OK message not sent\n");
						}
					}
				}
			}
		}

		// //RECEIVE MESSAGE FROM EDITOR
		// if(msgrcv(msgidEditor, &dataToUpdate, sizeof(struct message),0,IPC_NOWAIT)== -1){
		// 	// printf("Error msgrcv failed");	
		// }else{
		// 	// DATA FROM DBEDITOR
		// 	printf("Data Received from DBEDITOR is : %s %s %f %s\n",dataToUpdate.pin,dataToUpdate.accountnumber,dataToUpdate.balance,dataToUpdate.text);

		// 	if(strcmp(dataToUpdate.text,"UPDATE_DB") == 0){
		// 		printf("DATABASE HAS BEEN UPDATED WITH:\n\tPIN: %s\n\tAccount Number: %s\n\tBalance:$%.2f\n\t%s\n",dataToUpdate.pin,dataToUpdate.accountnumber,dataToUpdate.balance,dataToUpdate.text);

		// 		fprintf(dbfile,"%s\t%s\t%f\n",
		// 			dataToUpdate.pin,
		// 			dataToUpdate.accountnumber,
		// 			dataToUpdate.balance);
		// 		//add 1 to pin number to encrypt
		// 	}
		
		// }
	}
	

}
