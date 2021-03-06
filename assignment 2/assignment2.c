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

// PCB STRUCTURE
struct process{
    int pid; // pid
    int arrivalTime; // arrival time
    int cpuTime; //cpu time
    int ioFreq; // io frequency
    int ioDuration; // io duration
    int partition;
    char oldState[11];
    char currentState[11]; //state of process

    int cpuCounter; // increments up as soon as process starts running
    int ioDurationCounter; // increments when process is in waiting
    int ioFrequencyCounter;
    int priority;
    int mem;
	int waittime;
	int turnaround;
	int starttime;
	int stoptime;
};

// STRUCTURE FOR MEMORY PARTITIONS
struct partition{
    int id;
    int size;
    bool available;
};

// QUEUEING STRUCTURE
struct Queue { 
    int front, rear, size; 
    unsigned capacity; 
    int* array; 
}; 

// int remainintMEM(){

// }

// QUEUEING FUNCTIONS
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
    // printf("Process %d enqueued to queue\n", (item+1)); 
} 
int dequeue(struct Queue* queue) 
{ 
    if (isEmpty(queue)) 
        return INT_MIN; 
    int item = queue->array[queue->front]; 
    queue->front = (queue->front + 1) 
                   % queue->capacity; 
    queue->size = queue->size - 1;
    // printf("Process %d dequeued\n", (item+1)); 
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
	float totalturnaround = 0;
	float totalwait = 0;
    float totalcpuTime = 0;
	
    fptr = fopen(argv[1], "r");
	outfile = fopen(argv[2], "w"); //create and open output.txt file to writereate and open output.txt file to write

    while(fgets(line[i], 200, fptr)) 
    {
        line[i][strlen(line[i]) - 1] = '\0';
        i++;
    }
    
    numCommands = i;
    struct process arr_process[numCommands];
    printf("Number of commands: %i\n",numCommands);

    for(i=0;i<numCommands;i++)
    {
        sscanf(line[i] , "%i %i %i %i %i %i %i", &arr_process[i].pid, &arr_process[i].priority, &arr_process[i].arrivalTime, &arr_process[i].cpuTime, &arr_process[i].ioFreq, &arr_process[i].ioDuration, &arr_process[i].mem);
        strcpy(arr_process[i].currentState,"NEW");
        arr_process[i].ioDurationCounter=0;
        arr_process[i].ioFrequencyCounter=0;
        arr_process[i].cpuCounter=0;
        totalcpuTime += arr_process[i].cpuTime;
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
    

// FCFS WITHOUT IO
    if (strcmp(argv[3],"fcfs")==0){

        struct Queue* queue = createQueue(numCommands); 
        
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
                    
                    if(arr_process[i].cpuCounter==arr_process[i].cpuTime){
						arr_process[i].stoptime = clock;
                        strcpy(arr_process[i].oldState,"RUNNING");
                        strcpy(arr_process[i].currentState,"TERMINATED");
                        completedCommands++;
                        threadAvail = true;
                        printf("%i\t%i\t%s\t%s\n",clock,arr_process[i].pid,arr_process[i].oldState,arr_process[i].currentState);
                        fprintf(outfile,"%i\t%i\t%s\t%s\n",clock,arr_process[i].pid,arr_process[i].oldState,arr_process[i].currentState);

						arr_process[i].turnaround = arr_process[i].stoptime - arr_process[i].arrivalTime;				
						totalturnaround = totalturnaround + arr_process[i].turnaround;
					
                        if(completedCommands==numCommands){
						
							float averageturnaround = totalturnaround/numCommands;
							float averagewait = totalwait/numCommands;
							printf("average turnaround, average wait %f\t%f\n",averageturnaround, averagewait);
                            exit(0);
                        }
                    }
                }
            }

            if(strcmp(arr_process[0].currentState,"READY")==0 && start){
                start = false;
                running = true;
                threadAvail = false;
                strcpy(arr_process[front(queue)].oldState,"READY");
                strcpy(arr_process[front(queue)].currentState,"RUNNING");
                printf("%i\t%i\t%s\t%s\n",clock,arr_process[currentProcess].pid,arr_process[front(queue)].oldState,arr_process[currentProcess].currentState);
                fprintf(outfile,"%i\t%i\t%s\t%s\n",clock,arr_process[currentProcess].pid,arr_process[front(queue)].oldState,arr_process[currentProcess].currentState);
                dequeue(queue);
            }

            if (running && threadAvail){
				arr_process[i].starttime = clock;
				arr_process[i].waittime = arr_process[i].starttime - arr_process[i].cpuTime;
				totalwait = totalwait + arr_process[i].waittime;
                if (!isEmpty(queue)){
                    strcpy(arr_process[front(queue)].oldState,arr_process[front(queue)].currentState);
                    strcpy(arr_process[front(queue)].currentState,"RUNNING");
                    printf("%i\t%i\t%s\t%s\n",clock,arr_process[front(queue)].pid,arr_process[front(queue)].oldState,arr_process[front(queue)].currentState);
                    fprintf(outfile,"%i\t%i\t%s\t%s\n",clock,arr_process[front(queue)].pid,arr_process[front(queue)].oldState,arr_process[front(queue)].currentState);
                    dequeue(queue);
                    threadAvail = false;
                }

            }
			


            clock ++;
        }
		
    }
// FCFS WITH IO    
    if (strcmp(argv[3],"fcfsIO")==0){

        struct Queue* queue = createQueue(numCommands); 

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
                    arr_process[i].ioFrequencyCounter++;
                    
                    if(arr_process[i].cpuCounter==arr_process[i].cpuTime){
						arr_process[i].stoptime = clock;
                        strcpy(arr_process[i].oldState,"RUNNING");
                        strcpy(arr_process[i].currentState,"TERMINATED");
                        completedCommands++;
                        threadAvail = true;
                        printf("%i\t%i\t%s\t%s\n",clock,arr_process[i].pid,arr_process[i].oldState,arr_process[i].currentState);
                        fprintf(outfile,"%i\t%i\t%s\t%s\n",clock,arr_process[i].pid,arr_process[i].oldState,arr_process[i].currentState);
						arr_process[i].turnaround = arr_process[i].stoptime - arr_process[i].arrivalTime;				
						totalturnaround = totalturnaround + arr_process[i].turnaround;
                        if(completedCommands==numCommands){
							float averageturnaround = totalturnaround/numCommands;
							float averagewait = totalwait/numCommands;
							printf("average turnaround, average wait %f\t%f\n",averageturnaround, averagewait);
                            exit(0);
                        }
                    }

                    if(arr_process[i].ioFrequencyCounter==arr_process[i].ioFreq){
                        arr_process[i].ioFrequencyCounter = 0;
                        if (strcmp(arr_process[i].currentState,"RUNNING")==0){
                            strcpy(arr_process[i].oldState,"RUNNING");
                            strcpy(arr_process[i].currentState,"WAITING");
                            threadAvail = true;
                            printf("%i\t%i\t%s\t%s\n",clock,arr_process[i].pid,arr_process[i].oldState,arr_process[i].currentState);
                            fprintf(outfile,"%i\t%i\t%s\t%s\n",clock,arr_process[i].pid,arr_process[i].oldState,arr_process[i].currentState);
                        }
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
                    enqueue(queue,i);
                    printf("%i\t%i\t%s\t%s\n",clock,arr_process[i].pid,arr_process[i].oldState,arr_process[i].currentState);
                    fprintf(outfile,"%i\t%i\t%s\t%s\n",clock,arr_process[i].pid,arr_process[i].oldState,arr_process[i].currentState);
                }
                
            }

            if(strcmp(arr_process[0].currentState,"READY")==0 && start){
                start = false;
                running = true;
                currentProcess = 0;
                threadAvail = false;
                strcpy(arr_process[front(queue)].oldState,"READY");
                strcpy(arr_process[front(queue)].currentState,"RUNNING");
                printf("%i\t%i\t%s\t%s\n",clock,arr_process[front(queue)].pid,arr_process[front(queue)].oldState,arr_process[front(queue)].currentState);
                fprintf(outfile,"%i\t%i\t%s\t%s\n",clock,arr_process[front(queue)].pid,arr_process[front(queue)].oldState,arr_process[front(queue)].currentState);
                dequeue(queue);
            }

            if (running && threadAvail && isEmpty(queue)==false){
				arr_process[i].starttime = clock;
				arr_process[i].waittime = arr_process[i].starttime - arr_process[i].cpuTime;
				totalwait = totalwait + arr_process[i].waittime;
                currentProcess = front(queue);
                threadAvail = false;
                strcpy(arr_process[front(queue)].oldState,"READY");
                strcpy(arr_process[front(queue)].currentState,"RUNNING");
                printf("%i\t%i\t%s\t%s\n",clock,arr_process[front(queue)].pid,arr_process[front(queue)].oldState,arr_process[front(queue)].currentState);
                fprintf(outfile,"%i\t%i\t%s\t%s\n",clock,arr_process[front(queue)].pid,arr_process[front(queue)].oldState,arr_process[front(queue)].currentState);
                dequeue(queue);
            }

            clock ++;
        }
    }
// PRIORITY SCHEDULING
    if(strcmp(argv[3],"priority")==0){//to store the processes that are ready
        struct Queue* queue1 = createQueue(numCommands);  //queue for priotity 1
		struct Queue* queue2 = createQueue(numCommands);  //queue for priotity 2
		struct Queue* queue3 = createQueue(numCommands);  //queue for priotity 3
		
        while(1){
            // Start Processes
            for(i=0;i<numCommands;i++)
            {
                // check if the process is ready
                if(clock >= arr_process[i].arrivalTime && strcmp(arr_process[i].currentState,"NEW")==0){
                    strcpy(arr_process[i].oldState,"NEW");
                    strcpy(arr_process[i].currentState,"READY");

                    if(arr_process[i].priority ==1){
						 enqueue(queue1,i);
                        //  printf("added to priority 1\n");
					} else if(arr_process[i].priority ==2){
						 enqueue(queue2,i);
                        //  printf("added to priority 2\n");
					}else if(arr_process[i].priority ==3){
						 enqueue(queue3,i);
                        //  printf("added to priority 3\n");
					}

                    printf("%i\t%i\t%i\t%s\t%s\n",
                        clock,arr_process[i].pid,
                        arr_process[i].priority,
                        arr_process[i].oldState,
                        arr_process[i].currentState);
                        fprintf(outfile,"%i\t%i\t%i\t%s\t%s\n",
                        clock,arr_process[i].pid,
                        arr_process[i].priority,
                        arr_process[i].oldState,
                        arr_process[i].currentState);
                    // starts executing commands when first command is ready
                    if(i==0){
                        start = true;
                    }
                }

                // check if process is complete
                if(strcmp(arr_process[i].currentState,"RUNNING")==0){
                    arr_process[i].cpuCounter ++;
                    
                    if(arr_process[i].cpuCounter==arr_process[i].cpuTime){
						arr_process[i].stoptime = clock;
                        strcpy(arr_process[i].oldState,"RUNNING");
                        strcpy(arr_process[i].currentState,"TERMINATED");
                        completedCommands++;
                        threadAvail = true;

                        if(arr_process[i].priority==1){
                            dequeue(queue1);
                        }else if (arr_process[i].priority==2){
                            dequeue(queue2);
                        }else if (arr_process[i].priority==3){
                            dequeue(queue3);
                        }

                        printf("%i\t%i\t%i\t%s\t%s\n",
                        clock,arr_process[i].pid,
                        arr_process[i].priority,
                        arr_process[i].oldState,
                        arr_process[i].currentState);
                        fprintf(outfile,"%i\t%i\t%i\t%s\t%s\n",
                        clock,arr_process[i].pid,
                        arr_process[i].priority,
                        arr_process[i].oldState,
                        arr_process[i].currentState);
						arr_process[i].turnaround = arr_process[i].stoptime - arr_process[i].arrivalTime;				
						totalturnaround = totalturnaround + arr_process[i].turnaround;
                        if(completedCommands==numCommands){
							float averageturnaround = totalturnaround/numCommands;
							float averagewait = totalwait/numCommands;
							printf("average turnaround, average wait %f\t%f\n",averageturnaround, averagewait);
                            exit(0);
                        }
                    }
                }
            }

            if(strcmp(arr_process[0].currentState,"READY")==0 && start){
                start = false;
                running = true;
                strcpy(arr_process[0].oldState,"READY");
                strcpy(arr_process[0].currentState,"RUNNING");
                printf("%i\t%i\t%i\t%s\t%s\n",
                clock,arr_process[0].pid,
                arr_process[0].priority,
                arr_process[0].oldState,
                arr_process[0].currentState);
                fprintf(outfile,"%i\t%i\t%i\t%s\t%s\n",
                clock,arr_process[0].pid,
                arr_process[0].priority,
                arr_process[0].oldState,
                arr_process[0].currentState);
            }

            if (running && threadAvail){
				arr_process[i].starttime = clock;
				arr_process[i].waittime = arr_process[i].starttime - arr_process[i].cpuTime;
				totalwait = totalwait + arr_process[i].waittime;

                if(!isEmpty(queue1)){
                    strcpy(arr_process[front(queue1)].oldState,"READY");
                    strcpy(arr_process[front(queue1)].currentState,"RUNNING");
                    threadAvail = false;
                    printf("%i\t%i\t%i\t%s\t%s\n",
                        clock,arr_process[front(queue1)].pid,
                        arr_process[front(queue1)].priority,
                        arr_process[front(queue1)].oldState,
                        arr_process[front(queue1)].currentState);
                    fprintf(outfile,"%i\t%i\t%i\t%s\t%s\n",
                        clock,arr_process[front(queue1)].pid,
                        arr_process[front(queue1)].priority,
                        arr_process[front(queue1)].oldState,
                        arr_process[front(queue1)].currentState);
				} else if((!isEmpty(queue2))){
                    strcpy(arr_process[front(queue2)].oldState,"READY");
                    strcpy(arr_process[front(queue2)].currentState,"RUNNING");
                    threadAvail = false;
                    printf("%i\t%i\t%i\t%s\t%s\n",
                        clock,arr_process[front(queue2)].pid,
                        arr_process[front(queue2)].priority,
                        arr_process[front(queue2)].oldState,
                        arr_process[front(queue2)].currentState);
                    fprintf(outfile,"%i\t%i\t%i\t%s\t%s\n",
                        clock,arr_process[front(queue2)].pid,
                        arr_process[front(queue2)].priority,
                        arr_process[front(queue2)].oldState,
                        arr_process[front(queue2)].currentState);
                    
				} else if((!isEmpty(queue3))){
                    strcpy(arr_process[front(queue3)].oldState,"READY");
                    strcpy(arr_process[front(queue3)].currentState,"RUNNING");
                    threadAvail = false;
                    printf("%i\t%i\t%i\t%s\t%s\n",
                        clock,arr_process[front(queue3)].pid,
                        arr_process[front(queue3)].priority,
                        arr_process[front(queue3)].oldState,
                        arr_process[front(queue3)].currentState);
                    fprintf(outfile,"%i\t%i\t%i\t%s\t%s\n",
                        clock,arr_process[front(queue3)].pid,
                        arr_process[front(queue3)].priority,
                        arr_process[front(queue3)].oldState,
                        arr_process[front(queue3)].currentState);
				}

            }

            clock ++;
        }
    }
// ROUND ROBBIN SCHEDULING
    if (strcmp(argv[3],"rr")==0){

        // breaking the processes down into ms for RR 
        for(i=0;i<numCommands;i++){
            arr_process[i].arrivalTime = arr_process[i].arrivalTime * 1000;
            arr_process[i].cpuTime = arr_process[i].cpuTime *1000;
        }

        struct Queue* queue = createQueue(numCommands); 

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
						arr_process[i].stoptime = clock;
                        strcpy(arr_process[i].oldState,"RUNNING");
                        strcpy(arr_process[i].currentState,"TERMINATED");
                        completedCommands++;
                        threadAvail = true;
                        printf("%i\t%i\t%s\t%s\n",clock,arr_process[i].pid,arr_process[i].oldState,arr_process[i].currentState);
                        fprintf(outfile,"%i\t%i\t%s\t%s\n",clock,arr_process[i].pid,arr_process[i].oldState,arr_process[i].currentState);

						arr_process[i].turnaround = arr_process[i].stoptime - arr_process[i].arrivalTime;				
						totalturnaround = totalturnaround + arr_process[i].turnaround;
                        if(completedCommands==numCommands){
							float averageturnaround = totalturnaround/numCommands;
							float averagewait = totalwait/numCommands;
							printf("average turnaround: %fs\t average wait: %fs\n",averageturnaround/1000, averagewait/1000);
                            exit(0);
                        }
                    }
                }

                if (rrCounter==100){
                    rrCounter=0;
                    threadAvail = true;
                    if (strcmp(arr_process[i].currentState,"TERMINATED")!=0){
                        enqueue(queue,i);
                        strcpy(arr_process[i].oldState,"RUNNING");
                        strcpy(arr_process[i].currentState,"READY");
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

            if (running && threadAvail && isEmpty(queue)==false){
                if (arr_process[front(queue)].cpuCounter==0){
                    arr_process[front(queue)].starttime = clock;
				    arr_process[front(queue)].waittime = arr_process[front(queue)].starttime - arr_process[front(queue)].arrivalTime;
				    totalwait = totalwait + arr_process[front(queue)].waittime;
                }
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
// MEMORY MANAGEMENT
    if(strcmp(argv[3],"mm1")==0 || strcmp(argv[3],"mm2")==0){
        int totalMEM = 0;
        int availableMEM = 0;
        int usableMEM = 0;
        int usedMEM = 0;
        struct partition partition[4];
        for(i=0;i<4;i++){
            partition[i].available = true;
            partition[i].id = i+1;
        }
        if(strcmp(argv[3],"mm1")==0){
            partition[0].size = 500;
            partition[1].size = 250;
            partition[2].size = 150;
            partition[3].size = 100;
            totalMEM = partition[3].size + partition[2].size + partition[1].size + partition[0].size;
        }else if(strcmp(argv[3],"mm2")==0){
            partition[0].size = 350;
            partition[1].size = 300;
            partition[2].size = 300;
            partition[3].size = 50;
            totalMEM = partition[3].size + partition[2].size + partition[1].size + partition[0].size;
        }
        availableMEM = totalMEM;
        printf("O = available partition\nN = newly allocated partition\nX = occupied partition\n");
        printf("TIME\tPID\tPAST\tNOW\t\tP1(%i)\tP2(%i)\tP3(%i)\tP4(%i)\tUSED\tAVAIL\tUSABLE\n",partition[0].size,partition[1].size,partition[2].size,partition[3].size);

        struct Queue* queue = createQueue(numCommands); 

        while(1){
            // Start Processes
            for(i=0;i<numCommands;i++)
            {
                // check if the process is ready
                if(clock >= arr_process[i].arrivalTime && strcmp(arr_process[i].currentState,"NEW")==0){
                    if(arr_process[i].mem<=partition[0].size){
                        strcpy(arr_process[i].oldState,"NEW");
                        strcpy(arr_process[i].currentState,"READY");
                        enqueue(queue,i);
                    }else{
                        strcpy(arr_process[i].oldState,"NEW");
                        strcpy(arr_process[i].currentState,"CANTRUN"); // THE PROCESS CAN'T RUN BECAUSE IT IS TOO LARGE FOR ANY OF THE PARTITIONS
                        completedCommands++;
                        if(completedCommands==numCommands){
                            exit(0);
                        }
                    }
                    printf("%i\t%i\t%s\t%s\t\t%s\t%s\t%s\t%s\t%i\t%i\t%i\n",
                    clock,arr_process[i].pid,
                    arr_process[i].oldState,
                    arr_process[i].currentState,
                    partition[0].available? "O":"X",
                    partition[1].available? "O":"X",
                    partition[2].available? "O":"X",
                    partition[3].available? "O":"X",
                    usedMEM,
                    usableMEM,
                    availableMEM
                    );
                    fprintf(outfile,"%i\t%i\t%s\t%s\t\t%s\t%s\t%s\t%s\t%i\t%i\t%i\n",
                    clock,arr_process[i].pid,
                    arr_process[i].oldState,
                    arr_process[i].currentState,
                    partition[0].available? "O":"X",
                    partition[1].available? "O":"X",
                    partition[2].available? "O":"X",
                    partition[3].available? "O":"X",
                    usedMEM,
                    usableMEM,
                    availableMEM
                    );
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
                        partition[arr_process[i].partition-1].available=true;
                        usedMEM = usedMEM - arr_process[i].mem;
                        usableMEM = usableMEM - partition[arr_process[i].partition-1].size;
                        availableMEM = totalMEM-usableMEM;
                        completedCommands++;
                        printf("%i\t%i\t%s\t%s\t%s\t%s\t%s\t%s\t%i\t%i\t%i\n",
                            clock,arr_process[i].pid,
                            arr_process[i].oldState,
                            arr_process[i].currentState,
                            partition[0].available? "O":"X",
                            partition[1].available? "O":"X",
                            partition[2].available? "O":"X",
                            partition[3].available? "O":"X",
                            usedMEM,
                            usableMEM,
                            availableMEM
                        );
                        fprintf(outfile,"%i\t%i\t%s\t%s\t%s\t%s\t%s\t%s\t%i\t%i\t%i\n",
                            clock,arr_process[i].pid,
                            arr_process[i].oldState,
                            arr_process[i].currentState,
                            partition[0].available? "O":"X",
                            partition[1].available? "O":"X",
                            partition[2].available? "O":"X",
                            partition[3].available? "O":"X",
                            usedMEM,
                            usableMEM,
                            availableMEM
                        );
                        if(completedCommands==numCommands){
                            exit(0);
                        }
                    }
                }
            }

            if ((partition[0].available || partition[1].available || partition[2].available || partition[3].available) && !isEmpty(queue)){
                
                // currentProcess = nextProcess;
                // nextProcess = currentProcess +1;
                if(partition[3].available && arr_process[front(queue)].mem<= partition[3].size ){
                    strcpy(arr_process[front(queue)].oldState,arr_process[front(queue)].currentState);
                    strcpy(arr_process[front(queue)].currentState,"RUNNING");
                    arr_process[front(queue)].partition = 4;
                    partition[3].available = false;
                    usedMEM = usedMEM + arr_process[front(queue)].mem;
                    usableMEM = usableMEM + partition[3].size;
                    availableMEM = totalMEM-usableMEM;
                    printf("%i\t%i\t%s\t%s\t\t%s\t%s\t%s\t%s\t%i\t%i\t%i\n",
                        clock,arr_process[front(queue)].pid,
                        arr_process[front(queue)].oldState,
                        arr_process[front(queue)].currentState,
                        partition[0].available? "O":"X",
                        partition[1].available? "O":"X",
                        partition[2].available? "O":"X",
                        partition[3].available? "O":"N",
                        usedMEM,
                        usableMEM,
                        availableMEM
                    );

                    fprintf(outfile,"%i\t%i\t%s\t%s\t\t%s\t%s\t%s\t%s\t%i\t%i\t%i\n",
                        clock,arr_process[front(queue)].pid,
                        arr_process[front(queue)].oldState,
                        arr_process[front(queue)].currentState,
                        partition[0].available? "O":"X",
                        partition[1].available? "O":"X",
                        partition[2].available? "O":"X",
                        partition[3].available? "O":"N",
                        usedMEM,
                        usableMEM,
                        availableMEM
                    );
                    dequeue(queue);
                } 
                else if (partition[2].available && arr_process[front(queue)].mem<= partition[2].size ){
                    strcpy(arr_process[front(queue)].oldState,arr_process[front(queue)].currentState);
                    strcpy(arr_process[front(queue)].currentState,"RUNNING");
                    partition[2].available = false;
                    arr_process[front(queue)].partition = 3;
                    usedMEM = usedMEM + arr_process[front(queue)].mem;
                    usableMEM = usableMEM + partition[2].size;
                    availableMEM = totalMEM-usableMEM;
                    printf("%i\t%i\t%s\t%s\t\t%s\t%s\t%s\t%s\t%i\t%i\t%i\n",
                        clock,arr_process[front(queue)].pid,
                        arr_process[front(queue)].oldState,
                        arr_process[front(queue)].currentState,
                        partition[0].available? "O":"X",
                        partition[1].available? "O":"X",
                        partition[2].available? "O":"N",
                        partition[3].available? "O":"X",
                        usedMEM,
                        usableMEM,
                        availableMEM
                    );

                    fprintf(outfile,"%i\t%i\t%s\t%s\t\t%s\t%s\t%s\t%s\t%i\t%i\t%i\n",
                        clock,arr_process[front(queue)].pid,
                        arr_process[front(queue)].oldState,
                        arr_process[front(queue)].currentState,
                        partition[0].available? "O":"X",
                        partition[1].available? "O":"X",
                        partition[2].available? "O":"N",
                        partition[3].available? "O":"X",
                        usedMEM,
                        usableMEM,
                        availableMEM
                    );
                    dequeue(queue);
                } else if (partition[1].available && arr_process[front(queue)].mem<= partition[1].size ) {
                    strcpy(arr_process[front(queue)].oldState,arr_process[front(queue)].currentState);
                    strcpy(arr_process[front(queue)].currentState,"RUNNING");
                    partition[1].available = false;
                    arr_process[front(queue)].partition = 2;
                    usedMEM = usedMEM + arr_process[front(queue)].mem;
                    usableMEM = usableMEM + partition[1].size;
                    availableMEM = totalMEM-usableMEM;
                    printf("%i\t%i\t%s\t%s\t\t%s\t%s\t%s\t%s\t%i\t%i\t%i\n",
                        clock,arr_process[front(queue)].pid,
                        arr_process[front(queue)].oldState,
                        arr_process[front(queue)].currentState,
                        partition[0].available? "O":"X",
                        partition[1].available? "O":"N",
                        partition[2].available? "O":"X",
                        partition[3].available? "O":"X",
                        usedMEM,
                        usableMEM,
                        availableMEM
                    );

                    fprintf(outfile,"%i\t%i\t%s\t%s\t\t%s\t%s\t%s\t%s\t%i\t%i\t%i\n",
                        clock,arr_process[front(queue)].pid,
                        arr_process[front(queue)].oldState,
                        arr_process[front(queue)].currentState,
                        partition[0].available? "O":"X",
                        partition[1].available? "O":"N",
                        partition[2].available? "O":"X",
                        partition[3].available? "O":"X",
                        usedMEM,
                        usableMEM,
                        availableMEM
                    );
                    dequeue(queue);
                }else if (partition[0].available && arr_process[front(queue)].mem<= partition[0].size ) {
                    strcpy(arr_process[front(queue)].oldState,arr_process[front(queue)].currentState);
                    strcpy(arr_process[front(queue)].currentState,"RUNNING");
                    partition[0].available = false;
                    arr_process[front(queue)].partition = 1;
                    usedMEM = usedMEM + arr_process[front(queue)].mem;
                    usableMEM = usableMEM + partition[0].size;
                    availableMEM = totalMEM-usableMEM;
                    printf("%i\t%i\t%s\t%s\t\t%s\t%s\t%s\t%s\t%i\t%i\t%i\n",
                        clock,arr_process[front(queue)].pid,
                        arr_process[front(queue)].oldState,
                        arr_process[front(queue)].currentState,
                        partition[0].available? "O":"N",
                        partition[1].available? "O":"X",
                        partition[2].available? "O":"X",
                        partition[3].available? "O":"X",
                        usedMEM,
                        usableMEM,
                        availableMEM
                    );

                    fprintf(outfile,"%i\t%i\t%s\t%s\t\t%s\t%s\t%s\t%s\t%i\t%i\t%i\n",
                        clock,arr_process[front(queue)].pid,
                        arr_process[front(queue)].oldState,
                        arr_process[front(queue)].currentState,
                        partition[0].available? "O":"N",
                        partition[1].available? "O":"X",
                        partition[2].available? "O":"X",
                        partition[3].available? "O":"X",
                        usedMEM,
                        usableMEM,
                        availableMEM
                    );

                    dequeue(queue);
                }
            }

            clock ++;
        }

    }
    return 0;
}
