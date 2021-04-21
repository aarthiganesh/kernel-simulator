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
#include "semun.h"
#include "message.h"
#include "dbarray.h"
// A function that terminates when enter key is pressed
FILE *dbfile = NULL;


void update_db(){
	char line [128][10];
	int acctCounter = 0;
	int numAccounts;
	


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
	
		
		//every 1 minute loops through db and resets the clock

			printf("----Interest-----\n");
			for(int i=0;i<numAccounts;i++){
				if(dbArray[i].balance > 0.00){
					//account has a positive balance 
					float new_balance = (0.01*(dbArray[i].balance))+dbArray[i].balance;
					dbArray[i].balance = new_balance;
					printf("New balance: $%.2f\n",dbArray[i].balance);//DEBUG
				}
				else if(dbArray[i].balance > 0.00){
					float new_balance = dbArray[i].balance-(0.02*(dbArray[i].balance));
					dbArray[i].balance = new_balance;
					printf("New balance: $%.2f\n",dbArray[i].balance);//DEBUG
				}
			}
	
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

// The main program creates a delay of 60 seconds so that the interest will be calculated every 60 seconds
int main()
{
	int i;
	for (i = 0; i < 60; i++) {
        // delay of one second
        delay(1000);
        printf("%d seconds have passed\n", i + 1);
		if(i == 59){
			update_db();
			i =0;
		}

    }


		
	
    
}
