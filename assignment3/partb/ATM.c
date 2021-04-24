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

//defining the queue ids
#define ATMQUEUE 12345
#define DBSERVERQUEUE 11111
#define MAX_TEXT_SIZE 500

char inputpin[3];
char inputaccount[5];
char bankingoption[20];
float withdrawamount=0;
char recacct[5];
int counter = 0;
int inputvalid = 0;

struct message dataToSend;
struct message dataReceived;

void requestinput();
int validInput (char pin [3], char accountnumber[5]);

// semaphore variables
static int sem_id;

int main(){
	int msgidATM;
	int msgidServer;

	msgidATM = msgget((key_t)ATMQUEUE, 0666|IPC_CREAT);
	msgidServer = msgget((key_t)DBSERVERQUEUE, 0666|IPC_CREAT);
	sem_id = semget((key_t)1200, 1, 0666 | IPC_CREAT);

	if (!set_semvalue(sem_id)) {
		fprintf(stderr, "Failed to initialize semaphore\n");
		exit(EXIT_FAILURE);
	}

	while(1){
		requestinput();
		
		strcpy(dataToSend.text,"UPDATE");
		printf("Data sent from ATM to DBSERVER is : %s, %s, %s\n", dataToSend.pin, dataToSend.accountnumber, dataToSend.text);

		if (!sem_wait(sem_id)) exit(EXIT_FAILURE);

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

			// if(strcmp(bankingoption,"transfer")==0){
			// 	while(1){
			// 		printf("Amount to transfer: $");
			// 		scanf("%f", &withdrawamount);
			// 		printf("Account Number of Recipient: $");
			// 		scanf("%s", recacct);
					

			// 		if(strlen(recacct)==5 & withdrawamount>0){
			// 			dataToSend.withdrawal = withdrawamount;
			// 			strcpy(dataToSend.accountnumber,recacct);
			// 			break;
			// 		}
			// 	}
			// }
			

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
				printf("There are not enough funds on your account for this transaction.\n");
			}else{
				printf("\nTRANSACTION RECIEPT\nAcct Number: %s\nBalance: $%.2f\n\n", dataReceived.accountnumber, dataReceived.balance);
			}
		}else if (strcmp(dataReceived.text,"DNE")==0){
			printf("Account does not exist.\n");
		}
		if (!sem_release(sem_id)) exit(EXIT_FAILURE);

	}
	del_semvalue(sem_id);
}

void requestinput(){
	while(1){
		// REQUEST INPUT DATA FROM ATM USER
		printf("Enter account number: ");
		scanf("%s", inputaccount);
		printf("Enter pin number: ");
		scanf("%s", inputpin);

		if(validInput(inputpin,inputaccount)==1){
			//READ PIN NUMBER
			strcpy(dataToSend.pin,inputpin);
			//READ ACCOUNT NUMBER
			strcpy(dataToSend.accountnumber,inputaccount);
			break;
		}
	}
}

// validates if the format of the pin and acct number is correct
int validInput (char pin [3], char accountnumber[5]){
	if(strcmp(inputpin,"X")){
		exit(EXIT_FAILURE);
	}else if (strlen(pin)==3 & strlen(accountnumber)==5){
		return 1;
	}else{
		printf("INVALID INPUT VALUES: Pin must be 3 numbers, account number must be 5 numbers\n\n");
		return 0;
	}
}