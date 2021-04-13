#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <errno.h>
//defining the queue ids
#define ATMQUEUE 12345
#define DBSERVERQUEUE 54321
#define MAX_TEXT_SIZE 500

char inputpin[3];
char inputaccount[5];
char bankingoption[20];
int counter = 0;


//structure for message queue; data to transfer from atm to dbserver
//text for message, pin, account
struct msg_to_pass{
	char text[MAX_TEXT_SIZE];
	char pin[3];
	float balance;
	char accountnumber[5];
};

struct msg_to_pass dataToSend;
struct msg_to_pass dataReceived;

int requestinput(void){
	counter ++;
	printf("Enter pin number: ");
	scanf("%s", inputpin);
	printf("Enter account number: ");
	scanf("%s", inputaccount);
	//pin
	for(int k =0;k<3;k++){
		dataToSend.pin[k] = inputpin[k];
	}
	//account
	for(int j =0;j<5;j++){
		dataToSend.accountnumber[j] = inputaccount[j];
	}

}

int senddatatext(char sendtext[],int size){
	for(int y =0;y<size;y++){
		dataToSend.text[y] = sendtext[y];
	}
}


int main()
{
	int msgid; // Read
	int msgid2; // Write

	//Data structs to send and receive from message queue
	int sending;//flag to know if atm is sending data/receiving data
	
	//creating a message queue and returns identifier
	//this queue will be used to send data from atm to dbserver, dbserver will receive
	msgid = msgget((key_t)ATMQUEUE, 0666|IPC_CREAT);
	
	printf("msgid %d\n",msgid);
	//queue used to send data from dbserver to atm, atm will receive
	// msgid2 = msgget((key_t)DBSERVERQUEUE, 0666|IPC_CREAT);

	//to receive the first message
	//msgrcv(msgid2, &message, sizeof(message), 0, 0);
	
	//request input from user and send to dbserver
	requestinput();

	counter--;
	sending = 1;
	//dataToSend.text = "PIN";
	char data[] = {'P','I','N'};

	senddatatext(data,sizeof(data));

	if(msgsnd(msgid, &dataToSend, sizeof(struct msg_to_pass),0) == -1){
		printf("Error msgsnd failed\n");
	}
	
	//display message being sent
	printf("Data sent from atm to dbserver is : %s, %s, %s\n", dataToSend.pin, dataToSend.accountnumber, dataToSend.text);
	
	sending = 0;

	if(!sending){
		//dataToSend = {}; //clearing the struct so that we can send new data
		//read the message from dbserver
		msgrcv(msgid2, (void * ) & dataReceived, sizeof(struct msg_to_pass),0, 0); 
       
			//if message received is PIN_WRONG
			if(strcmp(dataReceived.text,"PIN_WRONG") == 0){
				if(counter < 3){
					requestinput();
				}else if(counter == 3){
					printf("Account is blocked");
				}			
			}

			if(strcmp(dataReceived.text,"OK") == 0){
				printf("Banking Options: Show Balance , Withdrawal ");
				printf("Choose a Banking option: ");
				scanf("%s", bankingoption);
			}

		
		sending = 1; //has done reading
	}

	
	
	if(sending){
		if(strcmp(bankingoption,"Show Balance")==0){	
			//dataToSend.text = "BALANCE";
			char data[] = {'B','A','L','A','N','C','E'};
			senddatatext(data,sizeof(data));
		}else if(strcmp(bankingoption,"Withdrawal")==0){
			//dataToSend.text = "WITHDRAW";
			char data[] = {'W','I','T','H','D','R','A','W'};
			senddatatext(data,sizeof(data));
		}
		msgsnd(msgid, &dataToSend, sizeof(struct msg_to_pass),0);
		printf("Data sent from atm to dbserver is :  %s\n", dataToSend.text);
		sending = 0;
	}
	
	if(!sending){
		msgrcv(msgid2, (void * ) & dataReceived, sizeof(struct msg_to_pass),0, 0);
		if(strcmp(dataReceived.text,"NSF")==0){
			printf("Not Enough Funds available");
		}
		if(strcmp(dataReceived.text,"FUNDS_OK")==0){
			//display new balance on screen to do
		}


	}
	 
}


