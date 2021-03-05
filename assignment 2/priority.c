if(strcmp(argv[3],"priority")==0){
		arr_process arrayprocess[10]; //to store the processes that are ready
		for(i=0;i<numCommands;i++)
		{
			sscanf(line[i] , "%i %i %i %i", &arr_process[i].pid, &arr_process[i].arrivalTime, &arr_process[i].cpuTime, &arr_process[i].priority);
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

	    struct Queue* queue1 = createQueue(1000);  //queue for priotity 1
		struct Queue* queue2 = createQueue(1000);  //queue for priotity 2
		struct Queue* queue3 = createQueue(1000);  //queue for priotity 3
		int queueid; //to know which queue to execute
		

        while(1){
            // Start Processes
            for(i=0;i<numCommands;i++)
            {
                // check if the process is ready
                if(clock >= arr_process[i].arrivalTime && strcmp(arr_process[i].currentState,"NEW")==0){
                    strcpy(arr_process[i].oldState,"NEW");
                    strcpy(arr_process[i].currentState,"READY");
					if(arr_process[i].priotity ==1){
						 enqueue(queue1,i);
					}
					if(arr_process[i].priotity ==2){
						 enqueue(queue2,i);
					}
					if(arr_process[i].priotity ==1){
						 enqueue(queue3,i);
					}

                   
                    printf("%i\t%i\t%s\t%s\n",clock,arr_process[i].pid,arr_process[i].oldState,arr_process[i].currentState);
                    fprintf(outfile,"%i\t%i\t%s\t%s\n",clock,arr_process[i].pid,arr_process[i].oldState,arr_process[i].currentState);
                    // starts executing commands when first command is ready
                    if(i==0){
                        start = true;
                    }
                }
				
				// cases for queues
				if(queue1.isEmpty != 0){
					queueid = 1;
				}else if((queue1.isEmpty() == 0) && (queue2.isEmpty != 0)){
					queueid = 2;
					
				}else if((queue1.isEmpty() == 0)&&(queue2.isEmpty() == 0)&&(queue3.isEmpty() != 0)){
					queueid = 3;
				}

	
	 			if(strcmp(arr_process[0].currentState,"READY")==0 && start){
	                start = false;
	                running = true;
	                currentProcess = 0;
	                strcpy(arr_process[currentProcess].oldState,"READY");
	                strcpy(arr_process[currentProcess].currentState,"RUNNING");
	                printf("%i\t%i\t%s\t%s\n",clock,arr_process[currentProcess].pid,arr_process[currentProcess].oldState,arr_process[currentProcess].currentState);
	                fprintf(outfile,"%i\t%i\t%s\t%s\n",clock,arr_process[currentProcess].pid,arr_process[currentProcess].oldState,arr_process[currentProcess].currentState);
					if(queueid == 1){
						dequeue(queue1);
					}else if(queueid == 2){
						dequeue(queue2);
					}else if(queueid == 3){
						dequeue(queue3);
					}
	                
	                threadAvail = false;
            	}

            	if (running && threadAvail){
					if(queueid == 1){
						dcurrentProcess = front(queue1);
					}else if(queueid == 2){
						currentProcess = front(queue2);
					}else if(queueid == 3){
						currentProcess = front(queue3);
					}
               		//currentProcess = front(queue);
                	threadAvail = false;
                	strcpy(arr_process[currentProcess].oldState,arr_process[currentProcess].currentState);
                	strcpy(arr_process[currentProcess].currentState,"RUNNING");
                	dequeue(queue);
                	printf("%i\t%i\t%s\t%s\n",clock,arr_process[currentProcess].pid,arr_process[currentProcess].oldState,arr_process[currentProcess].currentState);
                	fprintf(outfile,"%i\t%i\t%s\t%s\n",clock,arr_process[currentProcess].pid,arr_process[currentProcess].oldState,arr_process[currentProcess].currentState);
            }

				
                // check if process is complete
                if(strcmp(arr_process[i].currentState,"RUNNING")==0){
                    arr_process[i].cpuCounter ++;
                    //rrCounter ++;
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
                }





		

    }
