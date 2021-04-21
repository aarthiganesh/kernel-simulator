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
void calcNumAcct();

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

			calcNumAcct();
			struct dbarray dbArray[numAccounts];
			readDB(dbArray, numAccounts);

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