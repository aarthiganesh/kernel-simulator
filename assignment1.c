#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#define LSIZ 128 
#define RSIZ 10 

struct process{
    int pid; // pid
    int arrivalTime; // arrival time
    int cpuTime; //cpu time
    int ioFreq; // io frequency
    int ioDuration; // io duration
    char oldState[10];
    char currentState[10]; //state of process

    int cpuCounter; // increments up as soon as process starts running
    int ioCounter; // increments when process is in waiting
};

struct eventList{
    int clock;
    int pid;
    char oldState[10];
    char currentState[10];
};


int main()
{
    char line[RSIZ][LSIZ];
	char fname [20];
    FILE *fptr = NULL; 
    int i = 0;
    int j = 0;
    int numCommands = 0;
    int clock = 0;
    bool threadAvail = true; // is true when the process thread is available
    bool iohandle = false; //initially there are no interrupts 
    bool start = false;
    struct process temp;

    int currentProcess;
    int processOnHold;
    int nextProcess;
    int completedCommands = 0;
	

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
        sscanf(line[i] , "%i %i %i %i %i", &arr_process[i].pid, &arr_process[i].arrivalTime, &arr_process[i].cpuTime, &arr_process[i].ioFreq, &arr_process[i].ioDuration);
        strcpy(arr_process[i].currentState,"NEW");
        arr_process[i].ioCounter=0;
    }

    // bubble sort commands in order of arrival time
    for(i=0; i<numCommands; i++)
    {
        for(j=i; j<numCommands; j++)
        {
            if(arr_process[i].arrivalTime > arr_process[j].arrivalTime)
            {
                temp = arr_process[i];
                arr_process[i] = arr_process[j];
                arr_process[j] = temp;
            }
        }
    }

    while(clock<150){
        
        // Start Processes
        for(i=0;i<numCommands;i++)
        {
            // check if the process is ready
            if(clock >= arr_process[i].arrivalTime && strcmp(arr_process[i].currentState,"NEW")==0){
                strcpy(arr_process[i].currentState,"READY");
            }

            // check if process is complete
            if(arr_process[i].cpuCounter==arr_process[i].cpuTime){
                strcpy(arr_process[i].currentState,"TERMINATED");
            }
            // check if process needs to be in io

            if(arr_process[i].ioCounter==arr_process[i].ioDuration && strcmp(arr_process[i].currentState,"WAITING")){
                arr_process[i].ioCounter = 0;
                strcpy(arr_process[i].currentState,"READY");
            }

            if(arr_process[i].cpuCounter==arr_process[i].cpuTime && strcmp(arr_process[i].currentState,"RUNNING")){
                strcpy(arr_process[i].currentState,"TERMINATED");
                completedCommands ++;
            }

            if(strcmp(arr_process[i].currentState,"WAITING")){
                arr_process[i].ioCounter ++;
            }

            if(strcmp(arr_process[i].currentState,"RUNNING")){
                arr_process[i].cpuCounter ++;
            }
        }

        if(start && )


        // check if proces needs to go back to ready
        // check ready to running
        if(!threadAvail && !strcmp(arr_process[currentProcess],"TERMINATED")){
            arr_process[currentProcess].cpuCounter ++;
        }else{
            strcmp(arr_process[nextProcess],"READY");
            currentProcess = nextProcess;
        }


        // if (threadAvail){
        //     arr_process[currentProcess].
        // }
        clock ++;
    }
    return 0;
}


// find the process with the lowest start time (counter: remaining time, interrupt intervals)
// run first process
// when there's an interrupt (counter: interrupt time)
// switch processes to next available process (next lowest start time)
// loop 