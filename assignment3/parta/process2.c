#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

//#define SHM_KEY 1234

struct shared_variable{
	char var;
}

//process 2
int main()
{
	char *message;
	char input;
	int shmid;
  	//struct shmseg *shmp;
	char (*variable)[20];
	key_t key = ftok("shmfile",65);
	shmid = shmget(key, 1024, 0644|IPC_CREAT);
	printf("id %d\n",shmid);
	if (shmid == -1) {
		perror("Shared memory");
		return 1;
	}

	//to read from memory
	//shmat to attach to shared memory
	variable = shmat(shmid,(void*)0,0);
	printf("Data read from common variable: %s\n",variable);
	message = "I am Process 2";
	puts(message);

	while(1){
		printf("Input value: ");
		scanf("%c", &input);
		
		switch(input){
		case 'x':
			message = "Finishes";
			puts(message);
			exit(0);
		case '2':
			message = "I am Process 2";
			puts(message);
			
		}
		//shared memory can have 1,2 or x
		
		if(strcmp(variable[0],'x') == 0){
			message = "Finishes";
			puts(message);
			exit(0);
		}else if(strcmp(variable[0],'2') == 0){
			message = "I am Process 2";
			puts(message);
		
		}
		// exit(0);
	}
	
		//detach memory
		shmdt(variable);
		//destroy shared memory
		shmctl(shmid,IPC_RMID,NULL);
	
}

