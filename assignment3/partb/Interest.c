#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <errno.h>
#include <sys/sem.h>

#include "semaphores.h"
#include "message.h"
#include "dbarray.h"
// A function that terminates when enter key is pressed
FILE *dbfile = NULL;
char line [128][100];
int numAccounts = 0;

void printOption(struct message dataReceived);
void readDB(struct dbarray dbArray[], int numAccounts);
void updateDB(struct dbarray dbArray[], int numAccounts);
void printDB(struct dbarray dbArray[], int numAccounts);
void delay(int number_of_seconds);
void addInterest( struct dbarray dbArray[], int numAccounts);
void calcNumAcct();
int validInput (char pin[3], char accountnumber[5], float funds);

// semaphore variables
static int sem_id;

// The main program creates a delay of 60 seconds so that the interest will be calculated every 60 seconds
int main()
{
	sem_id = semget((key_t)1200, 1, 0666 | IPC_CREAT);

	if (!set_semvalue(sem_id)) {
		fprintf(stderr, "Failed to initialize semaphore\n");
		exit(EXIT_FAILURE);
	}

	int i;
	for (i = 0; i < 60; i++) {
        // delay of one second
        delay(1000);
        // printf("%d seconds have passed\n", i + 1);//DEBUG
		if(i == 59){
			if (!sem_wait(sem_id)) exit(EXIT_FAILURE);
      calcNumAcct();
      struct dbarray dbArray[numAccounts];
      readDB(dbArray,numAccounts);
			addInterest(dbArray,numAccounts);
			i =0;
      numAccounts =0;
			if (!sem_release(sem_id)) exit(EXIT_FAILURE);	
		}
  }
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
	// printf("Number of Accounts: %i\n",numAccounts);
	for(int i=0;i<numAccounts;i++)
	{
			sscanf(line[i] , "%s %s %f\n", dbArray[i].accountnumber, dbArray[i].pin, &dbArray[i].balance);
			dbArray[i].count=0;
			// printf("Acct#:%s pin:%s balance: $%.2f\n", dbArray[i].accountnumber, dbArray[i].pin, dbArray[i].balance);//DEBUG
	}

	fclose(dbfile);
	// printf("---------------------END---------------------\n");
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

int validInput (char pin [3], char accountnumber[5], float funds){
	if (strlen(pin)==3 & strlen(accountnumber)==5 & funds>0){
		return 1;
	}else{
		printf("INVALID INPUT VALUES: Pin must be 3 numbers, account number must be 5 numbers, funds must be greater than $0.00\n");
		return 0;
	}
}

void printDB(struct dbarray dbArray[], int numAccounts){
	dbfile = fopen("db.txt", "r");

	printf("-------------CURRENT FILE CONTENTS-------------\n");
	printf("Number of Accounts: %i\n",numAccounts);
	for(int i=0;i<numAccounts;i++)
	{
			printf("Acct#:%s pin:%s balance: $%.2f\n", dbArray[i].accountnumber, dbArray[i].pin, dbArray[i].balance);//DEBUG
	}

	fclose(dbfile);
	printf("---------------------END---------------------\n");
}

void addInterest ( struct dbarray dbArray[], int numAccounts){
			printf("\nINTEREST NEW BALANCE\n");
			for(int i=0;i< numAccounts ;i++){
				if(dbArray[i].balance > 0.00){
					//account has a positive balance 
					// printf("Oldbalance: $%.2f\n",dbArray[i].balance);//DEBUG
					dbArray[i].balance = dbArray[i].balance * 1.01;
					printf("Account #%s: $%.2f\n",dbArray[i].accountnumber, dbArray[i].balance);//DEBUG
				}
				else if(dbArray[i].balance < 0.00){
					// printf("Oldbalance: $%.2f\n",dbArray[i].balance);//DEBUG
					dbArray[i].balance = dbArray[i].balance * 1.02;
					printf("Account #%s: $%.2f\n",dbArray[i].accountnumber, dbArray[i].balance);//DEBUG
				}
			}
      updateDB(dbArray,numAccounts);
	
}

void delay(int number_of_seconds)

{
    // Converting time into milli_seconds
    int milli_seconds = 1000 * number_of_seconds;
  
    // Storing start time
    clock_t start_time = clock();
  
    // looping till required time is not achieved
    while (clock() < start_time + milli_seconds)
        ;
}