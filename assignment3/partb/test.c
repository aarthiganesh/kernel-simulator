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
#define DBSERVERQUEUE 11111
#define DBEDITORQUEUE 54321
#define MAX_TEXT_SIZE 20

FILE *dbfile = NULL;

struct array{
	char pin[4];
	char accountnumber[6];
  float balance;
	int count;
};


int main()
{
	char line [128][10];
	int numAccounts;
	int acctCounter = 0;

	//Data structs to send and receive from message queue
	printf("DB File Open\n");
  dbfile = fopen("db.txt", "r+"); //create and open output.txt file to writereate and open output.txt file to write

	while(fgets(line[acctCounter], 200, dbfile)) 
	{
			acctCounter++;
	}
    
  numAccounts = acctCounter;
	struct array db[numAccounts];

	for(int i=0;i<numAccounts;i++)
	{
			sscanf(line[i] , "%s %s %f", db[i].accountnumber, db[i].pin, &db[i].balance);
			db[i].count=0;
			printf("Acct#:%s pin:%s balance: %f\n", db[i].accountnumber, db[i].pin, db[i].balance);//DEBUG
	}
}