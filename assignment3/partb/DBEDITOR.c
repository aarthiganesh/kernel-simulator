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

#define DBEDITORQUEUE 54321

FILE *dbfile = NULL;
char line [128][100];
int numAccounts = 0;

struct message dataToSend;

char inputpin[3];
char inputaccount[5];
float funds;

void printOption(struct message dataReceived);
void readDB(struct dbarray dbArray[], int numAccounts);
void updateDB(struct dbarray dbArray[], int numAccounts);
void printDB(struct dbarray dbArray[], int numAccounts);
void calcNumAcct();
int validInput (char pin[3], char accountnumber[5], float funds);

int main()
{
	int msgidEditor;
	int running = 1;
	int update;
	int actExists;
	msgidEditor = msgget((key_t)DBEDITORQUEUE, 0666|IPC_CREAT);
	

	printf("DBEDITOR QUEUE: %d\n",msgidEditor);
	
	while(running){
		printf("Enter pin number: ");
		scanf("%s", inputpin);
		printf("Enter account number: ");
		scanf("%s", inputaccount);
		printf("Enter funds available: ");
		scanf("%f",&funds);
		actExists = 0;
		update = 1;

		int valid = validInput(inputpin,inputaccount,funds);

		if(valid){
			calcNumAcct();
			struct dbarray dbArray[numAccounts];
			readDB(dbArray, numAccounts);
			printDB(dbArray, numAccounts);
			for(int i=0;i<numAccounts;i++){
				if(strcmp(inputaccount,dbArray[i].accountnumber) == 0){
					actExists = 1;
					dbArray[i].balance = funds;
					if(strcmp(inputpin,dbArray[i].pin) == 0){
						printf("BALANCE UPDATED\n");
						updateDB(dbArray,numAccounts);
						printDB(dbArray, numAccounts);
					}else{
						printf("PIN AND BALANCE UPDATED\n");
						strcpy(dbArray[i].pin,inputpin);
						updateDB(dbArray,numAccounts);
						printDB(dbArray, numAccounts);
					}
				}
			}
			if (actExists==0){
				struct dbarray updatedArray [numAccounts+1];
				for(int i=0;i<numAccounts;i++){
					updatedArray[i] = dbArray[i];
				}
				strcpy(updatedArray[numAccounts].pin,inputpin);
				strcpy(updatedArray[numAccounts].accountnumber,inputaccount);
				updatedArray[numAccounts].balance=funds;
				updateDB(updatedArray,(numAccounts+1));
				printDB(dbArray, numAccounts+1);
			}
			numAccounts = 0;
			actExists =0;
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