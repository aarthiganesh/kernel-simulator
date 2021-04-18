#include <sys/types.h>
#include<sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define SHM_KEY 0x1234

//process 1
int main()
{
	pid_t pid;
	char *args[] = {"./p2",NULL};
	char *message;
	char input;
	int shmid;
  struct shmseg *shmp;

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

			child_pid = wait(&stat_val);
			if(WIFEXITED(stat_val)){
				exit(0);
			}
		}else{
		//child process
			execv(args[0],args);
		}
	}
	
	
	exit(0);
}	

