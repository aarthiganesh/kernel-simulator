#include <stdio.h>
#include <sys/sem.h>

#include "semun.h"

//Set semaphore value
static int set_semvalue(int sem_id)
{
	union semun sem_union;
	sem_union.val = 1;
	if (semctl(sem_id, 0, SETVAL, sem_union) == -1) return(0);
	return(1);
}

//Delete semaphore
static void del_semvalue(int sem_id)
{
	union semun sem_union;
	if (semctl(sem_id, 0, IPC_RMID, sem_union) == -1)
		fprintf(stderr, "Failed to delete semaphore\n");
}

//Semaphore wait
static int sem_wait(int sem_id)
{
	struct sembuf sem_b;
	sem_b.sem_num = 0;
	sem_b.sem_op = -1; /* P() */
	sem_b.sem_flg = SEM_UNDO;
  printf("Waiting...\n");
	if (semop(sem_id, &sem_b, 1) == -1) {
		fprintf(stderr, "SEM: wait failed\n");
		return(0);
	}
	return(1);
}

//Semaphore release
static int sem_release(int sem_id)
{
	struct sembuf sem_b;
	sem_b.sem_num = 0;
	sem_b.sem_op = 1; /* V() */
	sem_b.sem_flg = SEM_UNDO;
	if (semop(sem_id, &sem_b, 1) == -1) {
		fprintf(stderr, "SEM: failed to release\n");
		return(0);
	}
	return(1);
}