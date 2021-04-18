#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define SHM_KEY 1234

struct shmseg {
   int input;
};

//process 2
int main()
{
	char *message;
	char input;
	int shmid;
  struct shmseg *shmp;

	shmid = shmget(SHM_KEY, sizeof(struct shmseg), 0644|IPC_CREAT);
	if (shmid == -1) {
		perror("Shared memory");
		return 1;
	}

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
			break;
		}
		// exit(0);
	}
	
}
