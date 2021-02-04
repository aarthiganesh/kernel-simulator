#include <stdio.h>
#include <stdlib.h>

int main(void){

    FILE *myFile;
    myFile = fopen("input.txt", "r");


    //creating a 2d array
    int pcbtable[5][2];
    int i,j;
	int smallPid;
	//int *arraypointer = pcbtable; 
	

    if (myFile == NULL){
        printf("Error Reading File\n");
        exit (0);
    }
	
	
	
    for (i = 0; i < 5; i++){
		for(j=0;j<2;j++){
        	fscanf(myFile, "%i", &pcbtable[i][j]);
			
    	}
		
	}

    for (i = 0; i < 5; i++){
		for(j=0;j<2;j++){
        	printf("%i\n",pcbtable[i][j]);
    	}
	}

   

	for(j=0;j<2;j++){

		if((pcbtable[1][j]) < (pcbtable[1][j+1])){
			smallPid = pcbtable[0][j];
			printf("smallpid: %i\n",smallPid);
			printf("smallpid: %i\n",pcbtable[1][j]);
		}

	}
	fclose(myFile);
	return 0;
	

}




