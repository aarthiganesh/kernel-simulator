#include <sys/types.h>
#include<sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
// #include "semun.h"
#define SHM_KEY 1234

//process 1
int main()
{
	pid_t pid;
	char *args[] = {"./process2",NULL};
	char *message;
	char input;
	int shmid;
	char(*variable)[20];
  	//struct shmseg *shmp;
	key_t key = ftok("shmfile",65);
	shmid = shmget(key, 1024, 0644|IPC_CREAT);
	
	printf("id %d\n",shmid);
	//to write in memory
	//shmat to attach to shared memory
	variable = shmat(shmid,(void*)0,0);
	printf("Enter common variable: ");
	scanf("%s",(*variable));
	printf("Data written in memory: %s\n",variable);
	
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

			if(strcmp(variable[0],'x') == 0){
				message = "Finishes";
				puts(message);
				exit(0);
			}else if(strcmp(variable[0],'2') == 0){
				message = "I am Process 2";
				puts(message);
		
			}

			child_pid = wait(&stat_val);
			if(WIFEXITED(stat_val)){
				exit(0);
			}
		}else{
		//child process
			execv(args[0],args);
		}
	}
		//detach from shared memory
		shmdt(variable);

	
	
	exit(0);
}