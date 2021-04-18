#include <sys/types.h>
#include<sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include "semun.h"
#define QUEUE 123456
#define QUEUE1 654321
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
struct msg_to_pass{
	int number;
};
//process 1
int main()
{
	pid_t pid;
	char *args[] = {"./process2",NULL};
	char *message;
	char input;
	int shmid;
	char(*variable)[20];
	int msgid;
	int msgid1;
	int startnumber= 1;
	struct msg_to_pass dataToSend;

	key_t key = ftok("shmfile",65);
	shmid = shmget(key, 1024, 0644|IPC_CREAT);
	sem_id = semget((key_t)1234,1,0666|IPC_CREAT);
	msgid = msgget((key_t)QUEUE, 0666|IPC_CREAT);
	msgid1 = msgget((key_t)QUEUE1, 0666|IPC_CREAT);
	printf("msgid %d\n",msgid);

	if (!set_semvalue()) {
		fprintf(stderr,"Failed to initialize semaphore\n");
		exit(EXIT_FAILURE);
	}
	sleep(2);
	
	pid = fork();
	
	while(1){
		if(pid == -1){
			perror("fork failed");
			exit(1);
		}
		if(pid !=0){
		//parent process
			message = "I am Process 1";
			puts(message);
			int stat_val;
			pid_t child_pid;
			if(!semaphore_p()) exit(EXIT_FAILURE);
				printf("id %d\n",shmid);
				//parent has semaphore, can to write in shared memory
				variable = shmat(shmid,(void*)0,0);
				printf("Parent has the semaphore\n");fflush(stdout);
				printf("Enter common variable: \n");
				scanf("%s",variable);
				
				printf("Data written in memory: %s\n",variable);
				
				printf("startnumber p1 %d\n",startnumber);
				startnumber++;
			//p1 sending number to p2
				dataToSend.number = startnumber;
				if(msgsnd(msgid, &dataToSend, sizeof(struct msg_to_pass),0) == -1){
					printf("Error msgsnd failed\n");
				}
			printf("Data sent from p1 to p2 is :  %d\n", dataToSend.number);
			if(!semaphore_v()) exit(EXIT_FAILURE);
			break;
			
			//if(strcmp(variable[0],'x') == 0){
				//message = "Finishes";
				//puts(message);
				//exit(0);
			//}else if(strcmp(variable[0],'2') == 0){
				//message = "I am Process 2";
				//puts(message);
		
			//}
			sleep(2);

			child_pid = wait(&stat_val);
			if(WIFEXITED(stat_val)){
				exit(0);
			}
		}else{
		
		//first child process
			struct msg_to_pass dataReceived;
			struct msg_to_pass datatosend;
			sleep(5);
			if(!semaphore_p()) exit(EXIT_FAILURE);
			execv(args[0],args);
			
			printf("Child has the semaphore\n");
			
			fflush(stdout);
			if(!semaphore_v()) exit(EXIT_FAILURE);
			printf("\n%d - finished\n", getpid());
			sleep(10);
			if(msgrcv(msgid, &dataReceived, sizeof(struct msg_to_pass), 0, 0)== -1){
				printf("Error msgrcv failed");
			}

			// DATA RECEIVED FROM parent, new number is total
			printf("Data Received is : %d\n",dataReceived.number);
			int newnumber = dataReceived.number*2;
			int total = dataReceived.number+newnumber;
			printf("total p2 %d",total);

			if (msgctl(msgid, IPC_RMID, 0) == -1) {
				printf("Failed");	
			}

            del_semvalue();
			

			
		}
	}
		//detach from shared memory
		shmdt(variable);

	
	
	exit(0);
}	
