//-----------------------------------------------------------
//  SYSC 4001 SECTION B
//  ASSIGNMENT 1
//  Date: February 5, 2021

// Chhavi Sujeebun
// Student Number: 101126487

//  Aarth Ganesh
// Student Number: 100970339

//-----------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <limits.h> 

// //test cases
// #include "test.h"

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

// A structure to represent a queue 
struct Queue { 
    int front, rear, size; 
    unsigned capacity; 
    int* array; 
}; 
struct Queue* createQueue(unsigned capacity) 
        { 
            struct Queue* queue = (struct Queue*)malloc( 
                sizeof(struct Queue)); 
            queue->capacity = capacity; 
            queue->front = queue->size = 0; 
        
            // This is important, see the enqueue 
            queue->rear = capacity - 1; 
            queue->array = (int*)malloc( 
                queue->capacity * sizeof(int)); 
            return queue; 
        } 
int isFull(struct Queue* queue) 
{ 
    return (queue->size == queue->capacity); 
} 
int isEmpty(struct Queue* queue) 
{ 
    return (queue->size == 0); 
} 
void enqueue(struct Queue* queue, int item) 
{ 
    if (isFull(queue)) 
        return; 
    queue->rear = (queue->rear + 1) 
                  % queue->capacity; 
    queue->array[queue->rear] = item; 
    queue->size = queue->size + 1; 
    printf("%d enqueued to queue\n", item); 
} 
int dequeue(struct Queue* queue) 
{ 
    if (isEmpty(queue)) 
        return INT_MIN; 
    int item = queue->array[queue->front]; 
    queue->front = (queue->front + 1) 
                   % queue->capacity; 
    queue->size = queue->size - 1;
    printf("%d dequeued\n", item); 
    return item; 
}
int front(struct Queue* queue) 
{ 
    if (isEmpty(queue)) 
        return INT_MIN; 
    return queue->array[queue->front]; 
} 
int rear(struct Queue* queue) 
{ 
    if (isEmpty(queue)) 
        return INT_MIN; 
    return queue->array[queue->rear]; 
} 

struct eventList{
    int clock;
    int pid;
    char oldState[10];
    char currentState[10];
};

// arguments are argv[input.txt,output.txt]
int main(int argc,char *argv[])
{
    char line[RSIZ][LSIZ];
	char fname [20];
    FILE *fptr = NULL; 
	FILE *outfile = NULL;
    int i = 0;
    int j = 0;
    int numCommands = 0;
    int clock = 0;
    bool threadAvail = true; // is true when the process thread is available
    bool iohandle = false; //initially there are no interrupts 
    bool start = false;
    bool running = false;
    bool available = true;
    struct process temp;

    int currentProcess=0;
    int processOnHold=0;
    int nextProcess=0;
    int completedCommands = 0;
    int tempProcess=0;
    int rrCounter=0;
	

    fptr = fopen(argv[1], "r");
	outfile = fopen(argv[2], "w"); //create and open output.txt file to write

    while(fgets(line[i], 200, fptr)) 
    {
        line[i][strlen(line[i]) - 1] = '\0';
        i++;
    }
    
    numCommands = i;
    struct process arr_process[numCommands];

    // infinite loop, only breaks when all commands terminate
    if (strcmp(argv[3],"fcfs")==0){
        for(i=0;i<numCommands;i++)
        {
            sscanf(line[i] , "%i %i %i", &arr_process[i].pid, &arr_process[i].arrivalTime, &arr_process[i].cpuTime);
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

        printf("Number of commands: %i\n",numCommands);
        while(1){
            // Start Processes
            for(i=0;i<numCommands;i++)
            {
                // check if the process is ready
                if(clock >= arr_process[i].arrivalTime && strcmp(arr_process[i].currentState,"NEW")==0){
                    strcpy(arr_process[i].oldState,"NEW");
                    strcpy(arr_process[i].currentState,"READY");
                    printf("%i\t%i\t%s\t%s\n",clock,arr_process[i].pid,arr_process[i].oldState,arr_process[i].currentState);
                    fprintf(outfile,"%i\t%i\t%s\t%s\n",clock,arr_process[i].pid,arr_process[i].oldState,arr_process[i].currentState);
                    // starts executing commands when first command is ready
                    if(i==0){
                        start = true;
                    }
                }

                // check if process is complete
                if(strcmp(arr_process[i].currentState,"RUNNING")==0){
                    arr_process[i].cpuCounter ++;
                    
                    if(arr_process[i].cpuCounter==arr_process[i].cpuTime){
                        strcpy(arr_process[i].oldState,"RUNNING");
                        strcpy(arr_process[i].currentState,"TERMINATED");
                        completedCommands++;
                        threadAvail = true;
                        printf("%i\t%i\t%s\t%s\n",clock,arr_process[i].pid,arr_process[i].oldState,arr_process[i].currentState);
                        fprintf(outfile,"%i\t%i\t%s\t%s\n",clock,arr_process[i].pid,arr_process[i].oldState,arr_process[i].currentState);
                        if(completedCommands==numCommands){
                            exit(0);
                        }
                    }
                }
            }

            if(strcmp(arr_process[0].currentState,"READY")==0 && start){
                start = false;
                running = true;
                currentProcess = 0;
                nextProcess = 1;
                strcpy(arr_process[currentProcess].oldState,"READY");
                strcpy(arr_process[currentProcess].currentState,"RUNNING");
                printf("%i\t%i\t%s\t%s\n",clock,arr_process[currentProcess].pid,arr_process[currentProcess].oldState,arr_process[currentProcess].currentState);
                fprintf(outfile,"%i\t%i\t%s\t%s\n",clock,arr_process[currentProcess].pid,arr_process[currentProcess].oldState,arr_process[currentProcess].currentState);
            }

            if (running){

                if (strcmp(arr_process[currentProcess].currentState,"TERMINATED")==0 && strcmp(arr_process[nextProcess].currentState,"READY")==0 && currentProcess!=(numCommands-1)){
                    currentProcess = nextProcess;
                    nextProcess = currentProcess +1;
                    
                    strcpy(arr_process[currentProcess].oldState,arr_process[currentProcess].currentState);
                    strcpy(arr_process[currentProcess].currentState,"RUNNING");
                    printf("%i\t%i\t%s\t%s\n",clock,arr_process[currentProcess].pid,arr_process[currentProcess].oldState,arr_process[currentProcess].currentState);
                    fprintf(outfile,"%i\t%i\t%s\t%s\n",clock,arr_process[currentProcess].pid,arr_process[currentProcess].oldState,arr_process[currentProcess].currentState);
                }

            }

            clock ++;
        }
    }
    
    if (strcmp(argv[3],"fcfsIO")==0){
        
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

        printf("Number of commands: %i\n",numCommands);
        while(1){
            // Start Processes
            for(i=0;i<numCommands;i++)
            {
                // check if the process is ready
                if(clock >= arr_process[i].arrivalTime && strcmp(arr_process[i].currentState,"NEW")==0){
                    strcpy(arr_process[i].oldState,"NEW");
                    strcpy(arr_process[i].currentState,"READY");
                    printf("%i\t%i\t%s\t%s\n",clock,arr_process[i].pid,arr_process[i].oldState,arr_process[i].currentState);
                    fprintf(outfile,"%i\t%i\t%s\t%s\n",clock,arr_process[i].pid,arr_process[i].oldState,arr_process[i].currentState);
                    // starts executing commands when first command is ready
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
                        completedCommands++;
                        threadAvail = true;
                        printf("%i\t%i\t%s\t%s\n",clock,arr_process[i].pid,arr_process[i].oldState,arr_process[i].currentState);
                        fprintf(outfile,"%i\t%i\t%s\t%s\n",clock,arr_process[i].pid,arr_process[i].oldState,arr_process[i].currentState);
                        if(completedCommands==numCommands){
                            exit(0);
                        }
                    }

                    if(arr_process[i].ioFrequencyCounter==arr_process[i].ioFreq){
                        arr_process[i].ioFrequencyCounter = 0;
                        strcpy(arr_process[i].oldState,"RUNNING");
                        strcpy(arr_process[i].currentState,"WAITING");
                        threadAvail = true;
                        printf("%i\t%i\t%s\t%s\n",clock,arr_process[i].pid,arr_process[i].oldState,arr_process[i].currentState);
                        fprintf(outfile,"%i\t%i\t%s\t%s\n",clock,arr_process[i].pid,arr_process[i].oldState,arr_process[i].currentState);
                    }
                }
                
                if(strcmp(arr_process[i].currentState,"WAITING")==0){
                    arr_process[i].ioDurationCounter ++;
                }

                // check if process needs to be in io
                if(arr_process[i].ioDurationCounter== (arr_process[i].ioDuration+1) && strcmp(arr_process[i].currentState,"WAITING")==0){
                    arr_process[i].ioDurationCounter = 0;
                    strcpy(arr_process[i].oldState,"WAITING");
                    strcpy(arr_process[i].currentState,"RUNNING");
                    printf("%i\t%i\t%s\t%s\n",clock,arr_process[i].pid,arr_process[i].oldState,arr_process[i].currentState);
                    fprintf(outfile,"%i\t%i\t%s\t%s\n",clock,arr_process[i].pid,arr_process[i].oldState,arr_process[i].currentState);
                }
                
            }

            if(strcmp(arr_process[0].currentState,"READY")==0 && start){
                start = false;
                running = true;
                currentProcess = 0;
                nextProcess = 1;
                strcpy(arr_process[currentProcess].oldState,"READY");
                strcpy(arr_process[currentProcess].currentState,"RUNNING");
                printf("%i\t%i\t%s\t%s\n",clock,arr_process[currentProcess].pid,arr_process[currentProcess].oldState,arr_process[currentProcess].currentState);
                fprintf(outfile,"%i\t%i\t%s\t%s\n",clock,arr_process[currentProcess].pid,arr_process[currentProcess].oldState,arr_process[currentProcess].currentState);
            }

            if (running){

                if (strcmp(arr_process[currentProcess].currentState,"TERMINATED")==0 && strcmp(arr_process[nextProcess].currentState,"READY")==0 && currentProcess!=(numCommands-1)){
                    currentProcess = nextProcess;
                    nextProcess = currentProcess +1;
                    
                    strcpy(arr_process[currentProcess].oldState,arr_process[currentProcess].currentState);
                    strcpy(arr_process[currentProcess].currentState,"RUNNING");
                    printf("%i\t%i\t%s\t%s\n",clock,arr_process[currentProcess].pid,arr_process[currentProcess].oldState,arr_process[currentProcess].currentState);
                    fprintf(outfile,"%i\t%i\t%s\t%s\n",clock,arr_process[currentProcess].pid,arr_process[currentProcess].oldState,arr_process[currentProcess].currentState);
                }

            }

            clock ++;
        }
    }
    
    if (strcmp(argv[3],"rr")==0){
        for(i=0;i<numCommands;i++)
        {
            sscanf(line[i] , "%i %i %i", &arr_process[i].pid, &arr_process[i].arrivalTime, &arr_process[i].cpuTime);
            strcpy(arr_process[i].currentState,"NEW");
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

        printf("Number of commands: %i\n",numCommands);


        struct Queue* queue = createQueue(1000); 

        while(1){
            // Start Processes
            for(i=0;i<numCommands;i++)
            {
                // check if the process is ready
                if(clock >= arr_process[i].arrivalTime && strcmp(arr_process[i].currentState,"NEW")==0){
                    strcpy(arr_process[i].oldState,"NEW");
                    strcpy(arr_process[i].currentState,"READY");
                    enqueue(queue,i);
                    printf("%i\t%i\t%s\t%s\n",clock,arr_process[i].pid,arr_process[i].oldState,arr_process[i].currentState);
                    fprintf(outfile,"%i\t%i\t%s\t%s\n",clock,arr_process[i].pid,arr_process[i].oldState,arr_process[i].currentState);
                    // starts executing commands when first command is ready
                    if(i==0){
                        start = true;
                    }
                }

                // check if process is complete
                if(strcmp(arr_process[i].currentState,"RUNNING")==0){
                    arr_process[i].cpuCounter ++;
                    rrCounter ++;
                    threadAvail = false;
                    
                    if(arr_process[i].cpuCounter==arr_process[i].cpuTime){
                        strcpy(arr_process[i].oldState,"RUNNING");
                        strcpy(arr_process[i].currentState,"TERMINATED");
                        completedCommands++;
                        threadAvail = true;
                        printf("%i\t%i\t%s\t%s\n",clock,arr_process[i].pid,arr_process[i].oldState,arr_process[i].currentState);
                        fprintf(outfile,"%i\t%i\t%s\t%s\n",clock,arr_process[i].pid,arr_process[i].oldState,arr_process[i].currentState);
                        if(completedCommands==numCommands){
                            exit(0);
                        }
                    }

                    if(rrCounter==100 && strcmp(arr_process[0].currentState,"RUNNING")==0){
                        rrCounter=0;
                        threadAvail = true;
                        arr_process[i].ioFrequencyCounter = 0;
                        strcpy(arr_process[i].oldState,"RUNNING");
                        strcpy(arr_process[i].currentState,"READY");
                        enqueue(queue,i);
                        printf("%i\t%i\t%s\t%s\n",clock,arr_process[i].pid,arr_process[i].oldState,arr_process[i].currentState);
                        fprintf(outfile,"%i\t%i\t%s\t%s\n",clock,arr_process[i].pid,arr_process[i].oldState,arr_process[i].currentState);
                    }
                }
            }

            if(strcmp(arr_process[0].currentState,"READY")==0 && start){
                start = false;
                running = true;
                currentProcess = 0;
                strcpy(arr_process[currentProcess].oldState,"READY");
                strcpy(arr_process[currentProcess].currentState,"RUNNING");
                printf("%i\t%i\t%s\t%s\n",clock,arr_process[currentProcess].pid,arr_process[currentProcess].oldState,arr_process[currentProcess].currentState);
                fprintf(outfile,"%i\t%i\t%s\t%s\n",clock,arr_process[currentProcess].pid,arr_process[currentProcess].oldState,arr_process[currentProcess].currentState);
                dequeue(queue);
                threadAvail = false;
            }

            if (running && threadAvail){
                currentProcess = front(queue);
                threadAvail = false;
                strcpy(arr_process[currentProcess].oldState,arr_process[currentProcess].currentState);
                strcpy(arr_process[currentProcess].currentState,"RUNNING");
                dequeue(queue);
                printf("%i\t%i\t%s\t%s\n",clock,arr_process[currentProcess].pid,arr_process[currentProcess].oldState,arr_process[currentProcess].currentState);
                fprintf(outfile,"%i\t%i\t%s\t%s\n",clock,arr_process[currentProcess].pid,arr_process[currentProcess].oldState,arr_process[currentProcess].currentState);
            }

            clock ++;
        }
    }
    
    return 0;
}