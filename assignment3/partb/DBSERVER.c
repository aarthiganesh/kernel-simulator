#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/sem.h>

#include "message.h"
#include "dbarray.h"

//defining the queue ids
#define ATMQUEUE 12345
#define DBSERVERQUEUE 11111
#define DBEDITORQUEUE 54321
#define MAX_TEXT_SIZE 20

//file altering variables
FILE *dbfile = NULL;
char line [128][100];
int numAccounts = 0;

void printOption(struct message dataReceived);
void readDB(struct dbarray dbArray[], int numAccounts);
void updateDB(struct dbarray dbArray[], int numAccounts);
void calcNumAcct();

// message structs
struct message dataToSend;
struct message dataReceived;
struct message dataToUpdate;

// semaphore variables
static int sem_id;

static int set_semvalue(void);
static void del_semvalue(void);
static int sem_wait(void);
static int sem_release(void);


int main()
{
	int msgidATM;
	int msgidServer;
	int msgidEditor; // DBSERVER <--> DBEDITOR

	printf("Hello, DBSERVER\n");

	//this queue will be used to send data from atm to dbserver, dbserver will receive
	msgidATM = msgget((key_t)ATMQUEUE, 0666|IPC_CREAT);
	msgidServer = msgget((key_t)DBSERVERQUEUE, 0666|IPC_CREAT);
	msgidEditor = msgget((key_t)DBEDITORQUEUE, 0666|IPC_CREAT);
	sem_id = semget((key_t)1200, 1, 0666 | IPC_CREAT);

	if (!set_semvalue()) {
		fprintf(stderr, "Failed to initialize semaphore\n");
		exit(EXIT_FAILURE);
	}

	printf("ATM QID: %d\n",msgidATM);
	printf("DBSERVER QID: %d\n",msgidServer);
	printf("DBEDITOR QID: %d\n",msgidEditor);
	
	//to receive the first message
	while(1){

		//RECEIVE MESSAGE FROM ATM
		if(msgrcv(msgidATM, &dataReceived, sizeof(struct message),0,0)== -1){
			// printf("Error msgrcv failed");	
		}else{
			printf("----------------PIN and ACCT received-------------------\n");
			
			if (!sem_wait()) exit(EXIT_FAILURE);

			// read the db
			calcNumAcct();
			struct dbarray dbArray[numAccounts];
			readDB(dbArray, numAccounts);

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
						}else{
							printOption(dataReceived);
						}
						
						readDB(dbArray, numAccounts);
						if(strcmp(dataReceived.text,"balance")==0 ){
							printf("sending balance ...\n");
							dataToSend.balance = dbArray[i].balance;
						}else if (strcmp(dataReceived.text,"withdraw")==0 ){
							printf("WITHDRAWAL ATTEMPT: ");
							if(dataReceived.withdrawal<=dbArray[i].balance){
								printf("SUCCESSFUL\n");
								strcpy(dataToSend.text,"FUNDS_OK");
								dbArray[i].balance = dbArray[i].balance - dataReceived.withdrawal;
								dataToSend.balance = dbArray[i].balance;
								updateDB(dbArray,numAccounts);
								if(msgsnd(msgidEditor, &dataToSend, sizeof(struct message),0) == -1){
									printf("ERROR: message not send to editor\n");
								}else{
									printf("message sent to editor");
								}
							}else{
								strcpy(dataToSend.text,"NSF");
								printf("UNSUCCESSFUL, NSF\n");
							}
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
						if(msgsnd(msgidServer, &dataToSend, sizeof(struct message),0) == -1){
							printf("ERROR: OK message not sent\n");
						}
					}
				}
			}
			
			if (!sem_release()) exit(EXIT_FAILURE);

			numAccounts = 0;
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

	printf("-------------READING DB FILE CONTENTS-------------\n");
	printf("Number of Accounts: %i\n",numAccounts);
	for(int i=0;i<numAccounts;i++)
	{
			sscanf(line[i] , "%s %s %f\n", dbArray[i].accountnumber, dbArray[i].pin, &dbArray[i].balance);
			dbArray[i].count=0;
			printf("Acct#:%s pin:%s balance: $%.2f\n", dbArray[i].accountnumber, dbArray[i].pin, dbArray[i].balance);//DEBUG
	}

	fclose(dbfile);
	printf("---------------------END---------------------\n");
}

void updateDB(struct dbarray dbArray[], int numAccounts){

	dbfile = fopen("db.txt", "w");
	printf("Number of Accounts: %i\n",numAccounts);
	//Print contents of collection into the file with proper formatting
	for (int i = 0; i < numAccounts; i++) {
		fprintf(dbfile, "%s %s %.2f\n", dbArray[i].accountnumber, dbArray[i].pin, dbArray[i].balance);
	}
	//Close file and return
	fclose(dbfile);
}

//Set semaphore value
static int set_semvalue(void)
{
	union semun sem_union;
	sem_union.val = 1;
	if (semctl(sem_id, 0, SETVAL, sem_union) == -1) return(0);
	return(1);
}

//Delete semaphore
static void del_semvalue(void)
{
	union semun sem_union;
	if (semctl(sem_id, 0, IPC_RMID, sem_union) == -1)
		fprintf(stderr, "Failed to delete semaphore\n");
}

//Semaphore wait
static int sem_wait(void)
{
	struct sembuf sem_b;
	sem_b.sem_num = 0;
	sem_b.sem_op = -1; /* P() */
	sem_b.sem_flg = SEM_UNDO;
	if (semop(sem_id, &sem_b, 1) == -1) {
		fprintf(stderr, "SEM: wait failed\n");
		return(0);
	}
	return(1);
}

//Semaphore release
static int sem_release(void)
{
	struct sembuf sem_b;
	sem_b.sem_num = 0;
	sem_b.sem_op = 1; /* V() */
	sem_b.sem_flg = SEM_UNDO;
	if (semop(sem_id, &sem_b, 1) == -1) {
		fprintf(stderr, "SEM: failed to release\n");
		return(0);
	}
	return(1);
}