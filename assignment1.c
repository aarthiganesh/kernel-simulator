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
    char oldState[11];
    char currentState[11]; //state of process

    int cpuCounter; // increments up as soon as process starts running
    int ioDurationCounter; // increments when process is in waiting
    int ioFrequencyCounter;
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
    bool running = false;
    struct process temp;

    int currentProcess=0;
    int processOnHold=0;
    int nextProcess=0;
    int completedCommands = 0;
    int tempProcess=0;
	

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
        arr_process[i].ioDurationCounter=0;
        arr_process[i].ioFrequencyCounter=0;
        arr_process[i].cpuCounter=0;
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

    printf("%i",numCommands);

    while(clock<150){
        // printf("%i\t",clock);
        // Start Processes
        for(i=0;i<numCommands;i++)
        {
            // check if the process is ready
            if(clock >= arr_process[i].arrivalTime && strcmp(arr_process[i].currentState,"NEW")==0){
                strcpy(arr_process[i].oldState,"NEW");
                strcpy(arr_process[i].currentState,"READY");
                printf("%i\t%i\t%s\t%s\n",clock,arr_process[i].pid,arr_process[i].oldState,arr_process[i].currentState);
                if(i==0){
                    start = true;
                }
            }

            // check if process is complete
            if(strcmp(arr_process[i].currentState,"RUNNING")==0){
                arr_process[i].cpuCounter ++;
                arr_process[i].ioFrequencyCounter++;
                
                if(arr_process[i].cpuCounter==arr_process[i].cpuTime){
                    strcpy(arr_process[i].oldState,"RUNNING");
                    strcpy(arr_process[i].currentState,"TERMINATED");
                    threadAvail = true;
                    printf("%i\t%i\t%s\t%s\n",clock,arr_process[i].pid,arr_process[i].oldState,arr_process[i].currentState);
                }

                if(arr_process[i].ioFrequencyCounter==arr_process[i].ioFreq){
                    arr_process[i].ioFrequencyCounter = 0;
                    strcpy(arr_process[i].oldState,"RUNNING");
                    strcpy(arr_process[i].currentState,"WAITING");
                    threadAvail = true;
                    printf("%i\t%i\t%s\t%s\n",clock,arr_process[i].pid,arr_process[i].oldState,arr_process[i].currentState);
                }
                

            }

            if(strcmp(arr_process[i].currentState,"WAITING")==0){
                arr_process[i].ioDurationCounter ++;
            }

            // check if process needs to be in io
            if(arr_process[i].ioDurationCounter== (arr_process[i].ioDuration+1) && strcmp(arr_process[i].currentState,"WAITING")==0){
                arr_process[i].ioDurationCounter = 0;
                strcpy(arr_process[i].oldState,"WAITING");
                strcpy(arr_process[i].currentState,"READY");
                printf("%i\t%i\t%s\t%s\n",clock,arr_process[i].pid,arr_process[i].oldState,arr_process[i].currentState);
            }

            // if(arr_process[i].cpuCounter==arr_process[i].cpuTime && strcmp(arr_process[i].currentState,"RUNNING")){
            //     strcpy(arr_process[i].oldState,"RUNNING");
            //     strcpy(arr_process[i].currentState,"TERMINATED");
            //     completedCommands ++;
            //     printf("%i\t%i\t%s\t%s\r",clock,arr_process[i].pid,arr_process[i].oldState,arr_process[i].currentState);
            // }

        }

        if(strcmp(arr_process[0].currentState,"READY")==0 && start){
            start = false;
            running = true;
            currentProcess = 0;
            nextProcess = 1;
            processOnHold = 1;
            strcpy(arr_process[currentProcess].oldState,"READY");
            strcpy(arr_process[currentProcess].currentState,"RUNNING");
            threadAvail = false;
        }

        if (running){
            if(threadAvail){
                // printf("%i\t Thread is available\n",clock);
                if (strcmp(arr_process[processOnHold].currentState,"READY")==0){
                    threadAvail = false;
                    tempProcess = currentProcess;
                    currentProcess = processOnHold;
                    processOnHold = tempProcess;
                    strcpy(arr_process[currentProcess].oldState,"READY");
                    strcpy(arr_process[currentProcess].currentState,"RUNNING");
                    printf("%i\t%i\t%s\t%s\n",clock,arr_process[currentProcess].pid,arr_process[currentProcess].oldState,arr_process[currentProcess].currentState);
                }else if (strcmp(arr_process[nextProcess].currentState,"READY")==0){
                    printf("%i\t Thread is occupied\n",clock);
                    threadAvail = false;
                    currentProcess = nextProcess;
                    if(nextProcess<numCommands){
                        nextProcess ++;
                    }
                    strcpy(arr_process[currentProcess].oldState,"READY");
                    strcpy(arr_process[currentProcess].currentState,"RUNNING");
                    printf("%i\t%i\t%s\t%s\n",clock,arr_process[currentProcess].pid,arr_process[currentProcess].oldState,arr_process[currentProcess].currentState);
                }
            }
        }

        // printf("%i\t%i\t%s\t%s\n\n",clock,arr_process[0].pid,arr_process[0].oldState,arr_process[0].currentState);
        // printf("%i\t%i\t%s\t%s\n\n",clock,arr_process[1].pid,arr_process[1].oldState,arr_process[1].currentState);

        clock ++;
    }
    return 0;
}


// find the process with the lowest start time (counter: remaining time, interrupt intervals)
// run first process
// when there's an interrupt (counter: interrupt time)
// switch processes to next available process (next lowest start time)
// loop 