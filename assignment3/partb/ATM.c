#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <errno.h>

#include "message.h"

//defining the queue ids
#define ATMQUEUE 12345
#define DBSERVERQUEUE 11111
#define MAX_TEXT_SIZE 500

char inputpin[3];
char inputaccount[5];
char bankingoption[20];
float withdrawamount=0;
int counter = 0;
int inputvalid = 0;

struct message dataToSend;
struct message dataReceived;

void requestinput();

int main(){
	int msgidATM;
	int msgidServer;

	msgidATM = msgget((key_t)ATMQUEUE, 0666|IPC_CREAT);
	msgidServer = msgget((key_t)DBSERVERQUEUE, 0666|IPC_CREAT);

	while(1){
		requestinput();
		strcpy(dataToSend.text,"UPDATE");
		printf("Data sent from ATM to DBSERVER is : %s, %s, %s\n", dataToSend.pin, dataToSend.accountnumber, dataToSend.text);

		//ATM Sends pin and account info
		if(msgsnd(msgidATM, &dataToSend, sizeof(struct message),0) == -1){
			printf("Error: Sending pin and acct failed\n");
		}
		// atm recieves message from server (OK/PIN_ERROR)
		if(msgrcv(msgidServer, &dataReceived, sizeof(struct message),0,0)== -1){
			printf("Error: recieving OK/PIN_ERROR failed\n");	
		}
		printf("%s\n",dataReceived.text);

		if(strcmp(dataReceived.text,"OK")==0){
			printf("Please select banking option: ");
			scanf("%s", bankingoption);
			strcpy(dataToSend.text,bankingoption);

			if(strcmp(bankingoption,"withdraw")==0){
				printf("Amount to withdraw: $");
				scanf("%f", &withdrawamount);
				dataToSend.withdrawal = withdrawamount;
			}

			if(msgsnd(msgidATM, &dataToSend, sizeof(struct message),0) == -1){
				printf("Error: Sending pin and acct failed\n");
			}
			// recieve response
			if(msgrcv(msgidServer, &dataReceived, sizeof(struct message),0,0)== -1){
				printf("Error: recieving OK/PIN_ERROR failed\n");	
			}

			if(strcmp(dataReceived.text,"UNKNOWN OPTION")==0){
				printf("Urecognized Banking Option, please enter one of the following optoins:\n\tbalane\n\twithdraw\n");
			}else if (strcmp(dataReceived.text,"NSF")==0){
				printf("There are not enough funds on your account to withdraw.\n");
			}else{
				printf("----------------TRANSACTION RECIEPT----------------\nAcct Number: %s\nBalance: $%.2f\n", dataReceived.accountnumber, dataReceived.balance);
			}

		}

	}

}

void requestinput(){
  // REQUEST INPUT DATA FROM ATM USER
  printf("Enter pin number: ");
	scanf("%s", inputpin);
	printf("Enter account number: ");
	scanf("%s", inputaccount);

  //READ PIN NUMBER
	strcpy(dataToSend.pin,inputpin);
	//READ ACCOUNT NUMBER
	strcpy(dataToSend.accountnumber,inputaccount);
}