#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <errno.h>
#include <sys/sem.h>

#include "semun.h"
#include "message.h"
#include "semaphores.h"
#include "dbarray.h"

//defining the queue ids
#define ATMQUEUE 12346
#define DBSERVERQUEUE 11111
#define MAX_TEXT_SIZE 500

FILE *dbfile = NULL;
char line [128][100];
int numAccounts = 0;

char inputpin[3];
char inputaccount[5];
float funds;

int counter = 0;
int inputvalid = 0;

struct message dataToSend;

void requestinput();
void printOption(struct message dataReceived);
void readDB(struct dbarray dbArray[], int numAccounts);
void updateDB(struct dbarray dbArray[], int numAccounts);
void printDB(struct dbarray dbArray[], int numAccounts);
void calcNumAcct();
int validInput (char pin[3], char accountnumber[5], float funds);

// semaphore variables
static int sem_id;

int main(){
	int msgidEditor;

	msgidEditor = msgget((key_t)ATMQUEUE, 0666|IPC_CREAT);
	sem_id = semget((key_t)1200, 1, 0666 | IPC_CREAT);

	if (!set_semvalue(sem_id)) {
		fprintf(stderr, "Failed to initialize semaphore\n");
		exit(EXIT_FAILURE);
	}

	while(1){
		printf("Enter pin number: ");
		scanf("%s", inputpin);
		printf("Enter account number: ");
		scanf("%s", inputaccount);
		printf("Enter funds available: ");
		scanf("%f",&funds);

    int valid = validInput(inputpin,inputaccount,funds);

    if (valid){
      int actExists = 0;
      if (!sem_wait(sem_id)) exit(EXIT_FAILURE);


      calcNumAcct();
      struct dbarray dbArray[numAccounts];
			readDB(dbArray, numAccounts);

      for(int i=0;i<numAccounts;i++){
				// if accoutn exists
				if(strcmp(inputaccount,dbArray[i].accountnumber) == 0){
          actExists = 1;
					dbArray[i].balance = funds;
					// update balance
					if(strcmp(inputpin,dbArray[i].pin) == 0){
						printf("BALANCE UPDATED\n");
						updateDB(dbArray,numAccounts);
            // UPDATE MESSAGE SENT TO DBSERVER
            strcpy(dataToSend.text,"UPDATE");
            //EDITOR Sends update db
            if(msgsnd(msgidEditor, &dataToSend, sizeof(struct message),0) == -1){
              printf("Error: Sending pin and acct failed\n");
            }
					// both pin and balance are updated
					}else{
						printf("PIN AND BALANCE UPDATED\n");
						strcpy(dbArray[i].pin,inputpin);
						updateDB(dbArray,numAccounts);
            strcpy(dataToSend.text,"UPDATE");
            //EDITOR Sends update db
            if(msgsnd(msgidEditor, &dataToSend, sizeof(struct message),0) == -1){
              printf("Error: Sending pin and acct failed\n");
            }
					}
				}
			}

      	// account does not exist
			if (actExists==0){
				struct dbarray updatedArray [numAccounts+1];
				for(int i=0;i<numAccounts;i++){
					updatedArray[i] = dbArray[i];
				}
				strcpy(updatedArray[numAccounts].pin,inputpin);
				strcpy(updatedArray[numAccounts].accountnumber,inputaccount);
				updatedArray[numAccounts].balance=funds;
				updateDB(updatedArray,(numAccounts+1));

        strcpy(dataToSend.text,"UPDATE");
        //EDITOR Sends update db
        if(msgsnd(msgidEditor, &dataToSend, sizeof(struct message),0) == -1){
          printf("Error: Sending pin and acct failed\n");
        }
			}
			numAccounts = 0;
			actExists =0;

      if (!sem_release(sem_id)) exit(EXIT_FAILURE);
    }

	}
	del_semvalue(sem_id);
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

	printf("Reading db file contents...\n");
	for(int i=0;i<numAccounts;i++)
	{
			sscanf(line[i] , "%s %s %f\n", dbArray[i].accountnumber, dbArray[i].pin, &dbArray[i].balance);
			dbArray[i].count=0;
	}

	fclose(dbfile);
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

// validates if the input is correct
int validInput (char pin [3], char accountnumber[5], float funds){
	if (strlen(pin)==3 & strlen(accountnumber)==5 & funds>0){
		return 1;
	}else{
		printf("INVALID INPUT VALUES: Pin must be 3 numbers, account number must be 5 numbers, funds must be greater than $0.00\n");
		return 0;
	}
}

// gives a print statement with the current state of the DB
void printDB(struct dbarray dbArray[], int numAccounts){
	dbfile = fopen("db.txt", "r");

	printf("\nCURRENT FILE CONTENTS\n");
	printf("Number of Accounts: %i\n",numAccounts);
	for(int i=0;i<numAccounts;i++)
	{
			printf("Acct#:%s pin:%s balance: $%.2f\n", dbArray[i].accountnumber, dbArray[i].pin, dbArray[i].balance);//DEBUG
	}

	fclose(dbfile);
	printf("END\n");
}