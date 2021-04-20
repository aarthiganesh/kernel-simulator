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

//defining the queue ids
#define ATMQUEUE 12345
#define DBSERVERQUEUE 11111
#define MAX_TEXT_SIZE 500
//defining methods for semaphore
static int set_semvalue(void);
static void del_semvalue(void);
static int semaphore_p(void);
static int semaphore_v(void);
static int sem_id;

static int set_semvalue(void)
{
	union semun sem_union;
	sem_union.val = 1;
	if (semctl(sem_id, 0, SETVAL, sem_union) == -1) return(0);
	return(1);
}

static void del_semvalue(void)
{
	union semun sem_union;
	if (semctl(sem_id, 0, IPC_RMID, sem_union) == -1)
	fprintf(stderr,"Failed to delete semaphore\n");
}
static int semaphore_p(void)
{
	struct sembuf sem_b;
	sem_b.sem_num = 0;
	sem_b.sem_op = -1; /* P() */
	sem_b.sem_flg = SEM_UNDO;
	if (semop(sem_id, &sem_b, 1) == -1) {
		fprintf(stderr,"semaphore_p failed\n");
		return(0);
	}
	return(1);
}
static int semaphore_v(void)
{
	struct sembuf sem_b;
	sem_b.sem_num = 0;
	sem_b.sem_op = 1; /* V() */
	sem_b.sem_flg = SEM_UNDO;
	if (semop(sem_id, &sem_b, 1) == -1) {
		fprintf(stderr,"semaphore_v failed\n");
		return(0);
	}
	return(1);
}

char inputpin[3];
char inputaccount[5];
char transferaccount[5];
char bankingoption[20];
float withdrawamount=0;
float billamount =0;
int counter = 0;
int inputvalid = 0;

struct message dataToSend;
struct message dataReceived;

void requestinput(void){
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


int atm(){

	int msgidATM;
	int msgidServer;
	char *args[] = {"./DBSERVER",NULL};
	sem_id = semget((key_t)1234,1,0644|IPC_CREAT);
	msgidATM = msgget((key_t)ATMQUEUE, 0666|IPC_CREAT);
	msgidServer = msgget((key_t)DBSERVERQUEUE, 0666|IPC_CREAT);
	
	while(1){
		
		if (!set_semvalue()) {
			fprintf(stderr,"Failed to initialize semaphore\n");
			exit(EXIT_FAILURE);
		}
		sleep(2);
		if(!semaphore_p()) exit(EXIT_FAILURE);
	
		
		//printf("ATM # %d has semaphore\n",getpid());

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
				printf("Amount to withdraw: ");
				scanf("%f", &withdrawamount);
				dataToSend.withdrawal = withdrawamount;
			}
			//ADDING NEW FUNCTIONALITY paybill
				printf("Bill amount: ");
				scanf("%f", &billamount);
				dataToSend.bill = billamount;
			}

			if(msgsnd(msgidATM, &dataToSend, sizeof(struct message),0) == -1){
				printf("Error: Sending pin and acct failed\n");
			}
			
			// recieve response
			if(msgrcv(msgidServer, &dataReceived, sizeof(struct message),0,0)== -1){
				printf("Error: recieving OK/PIN_ERROR failed\n");	
			}

			printf("Message: %s\nAcct Number: %s\nBalance: %.2f\n", dataReceived.text, dataReceived.accountnumber, dataReceived.balance);

		}
		
		if(!semaphore_v()) exit(EXIT_FAILURE);
		 del_semvalue();
			//break;
	}
	

}

int main(){
	//total 5 processes running concurrently
	//creating first child
	 int firstchild = fork();
	//creating second child
	 int secondchild = fork();
	//creating third child
	 int thirdchild = fork ();
	 int fourthchild = fork();
	
	//parent
	if (firstchild > 0 && secondchild > 0 && thirdchild>0 && fourthchild>0)
    {
		//if(!semaphore_p()) exit(EXIT_FAILURE);
		printf("ATM # %d has semaphore\n",getpid());
		atm();
		
			
		
	}
	//first child
	else if(firstchild == 0 && secondchild > 0 && thirdchild>0 && fourthchild>0){
		//if(!semaphore_p()) exit(EXIT_FAILURE);
		printf("ATM # %d has semaphore\n",getpid());
		atm();
		
	}
	//second child
	else if(firstchild > 0 && secondchild == 0 && thirdchild>0 && fourthchild>0){
		//if(!semaphore_p()) exit(EXIT_FAILURE);
		printf("ATM # %d has semaphore\n",getpid());
		atm();
		
	}
	//third child
	else if(firstchild > 0 && secondchild > 0 && thirdchild==0 && fourthchild>0){
		//if(!semaphore_p()) exit(EXIT_FAILURE);
		printf("ATM # %d has semaphore\n",getpid());
		atm();
	}
	
	//fourth child
	else if(firstchild > 0 && secondchild > 0 && thirdchild>0 && fourthchild==0){
		//if(!semaphore_p()) exit(EXIT_FAILURE);
		printf("ATM # %d has semaphore\n",getpid());
		atm();
		
	}





}
