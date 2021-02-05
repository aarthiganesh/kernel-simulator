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
    int ioDur; // io duration
    char state[10]; //state of process
	char oldstate[10];
	char newstate[10];

    int cpuCounter; // increments up as soon as process starts running
    int ioCounter; // increments when process is running so that interrupt is generated every 20 ticks
	
	bool iohandle; //initially there are no interrupts 
};


int main(void)
{
    char line[RSIZ][LSIZ];
	char fname [20];
    FILE *fptr = NULL; 
    int i = 0;
    int j = 0;
    int numCommands = 0;
    int termCommands = 0;
    int clock = 0;
	int ioduration =0;//increments when process is in waiting
    bool threadAvail = true; // is true when the process thread is available
	
	//int interruptclock =0;


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
		arr_process[i].ioCounter = 0;

		arr_process[i].iohandle = false;
		printf("%i %s \n",arr_process[i].pid,arr_process[i].state);
    }
	
	////find smallest arrivaltime to determine which process goes first
    //for(j=0; j<numCommands; j++)
     //{
     	//if(arr_process[j].pid < arr_process[j+1].pid)
        //{
        	//tostartprocess[j] = arr_process[j].pid;
            //tostartprocess[j+1] = arr_process[j+1].pid;
        //}
		//printf("array %i \n",tostartprocess[j]);
         
     //}

    //clock_t start, end;

   while(clock<50){
        //printf("%i",clock);
        // state changes from NEW to READY when clock hits Arrival Time
        // state changes from 
		
	
        for(i=0;i<numCommands;i++)
        {
            // check if the process is ready
			//using the first come first serve idea 
            if(clock >= arr_process[i].arrivalTime && strcmp(arr_process[i].state,"NEW")==0 &&(threadAvail)){
    			strcpy(arr_process[i].oldstate,"READY");
				strcpy(arr_process[i].newstate,"RUNNING");
				printf("%i %i %s %s\n",clock,arr_process[i].pid,arr_process[i].oldstate,arr_process[i].newstate);
				threadAvail = false;
			}
			while (threadAvail == false){
				arr_process[i].ioCounter++;
				clock++;
				
				if(arr_process[i].ioCounter == arr_process[i].ioFreq){ //interrupt generated every iofrequency
					arr_process[i].iohandle = true;
					printf("iocounter is %i %i \n",arr_process[i].pid,arr_process[i].ioCounter);//increments as soon as process is in running
					strcpy(arr_process[i].oldstate,"RUNNING"); // running waiting
					strcpy(arr_process[i].newstate,"WAITING");
					
					printf("%i %i %s %s\n",clock,arr_process[i].pid,arr_process[i].oldstate,arr_process[i].newstate);
					arr_process[i].ioCounter = 0;
					clock++;
					ioduration++; 
					printf("clock is %i \n",clock);
					threadAvail = true;	
				} 
			}
			
	//is not doing 2222 at 26, waiting for whole loop to be over!
				while(arr_process[i].iohandle == true  && ioduration < arr_process[i].ioDur){
					ioduration++; //
					clock++;
					printf("clock is %i",clock);
					printf("duration  is %i %i\n",arr_process[i].pid,ioduration);
				
					if((ioduration) == (arr_process[i].ioDur)){
						printf("ioduration %i \n",ioduration);
						strcpy(arr_process[i].oldstate,"WAITING");
						strcpy(arr_process[i].newstate,"READY");
						printf("%i %i %s %s\n",clock,arr_process[i].pid,arr_process[i].oldstate,arr_process[i].newstate);
						arr_process[i].iohandle = false;
						ioduration = 0;

					}
				}
			

				
			
				
			
				

            

            // check if process is complete
            if(arr_process[i].cpuCounter==arr_process[i].cpuTime){
                strcpy(arr_process[i].state,"TERMINATED");
                termCommands ++;
                // if(termCommands == numCommands){
                //     exit;
                // }
            }
            // check if process needs to be in io

            // check if proces needs to go back to ready
            // check ready to running
        }

        // Start Processes
        // for(){

        // }

        clock++;
		
		
		
    }
	if(clock == 100){
		exit(0);
	}
    
    return 0;
}




// find the process with the lowest start time (counter: remaining time, interrupt intervals)
// run first process
// when there's an interrupt (counter: interrupt time)
// switch processes to next available process (next lowest start time)
// loop 
