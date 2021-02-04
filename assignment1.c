#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define LSIZ 128 
#define RSIZ 10 

struct process{
    int pid;
    int arrivalTime;
    int cpuTime;
    int cpuCounter;
    int ioFreq;
    int ioDur;
    int ioCounter;
    char state[10];
};


int main()
{
    char line[RSIZ][LSIZ];
	char fname [20];
    FILE *fptr = NULL; 
    int i = 0;
    int j = 0;
    int numCommands = 0;
    double cpu_time_used;
    int clock = 0;
	


    fptr = fopen("input.txt", "r");

    while(fgets(line[i], 200, fptr)) 
    {
        line[i][strlen(line[i]) - 1] = '\0';
        i++;
    }
    
    numCommands = i;
    struct process arr_process[numCommands];

    for(i=0;i<numCommands;i++)
    {
        sscanf(line[i] , "%i %i %i %i %i", &arr_process[i].pid, &arr_process[i].arrivalTime, &arr_process[i].cpuTime, &arr_process[i].ioFreq, &arr_process[i].ioDur);
        strcpy(arr_process[i].state,"NEW");
    }

    clock_t start, end;

    while(1){
        printf("%i",clock);
        for(i=0;i<numCommands;i++)
        {

            if(clock >= arr_process[i].arrivalTime && strcmp(arr_process[i].state,"NEW")==0){
                strcpy(arr_process[i].state,"READY");
            }
        }
        clock++;
    }
    
    return 0;
}


// find the process with the lowest start time (counter: remaining time, interrupt intervals)
// run first process
// when there's an interrupt (counter: interrupt time)
// switch processes to next available process (next lowest start time)
// loop 