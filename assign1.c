#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LSIZ 128 
#define RSIZ 10 

int main()
{
  char line[RSIZ][LSIZ];
	char fname [20];
  FILE *fptr = NULL; 
  int i = 0;
  int j = 0;
  int tot = 0;
  printf("\n\n Read the file and store the lines into an array :\n");
  printf("------------------------------------------------------\n"); 
  printf(" Input the filename to be opened : ");
  scanf("%s",fname);
	

  fptr = fopen(fname, "r");

  while(fgets(line[i], LSIZ, fptr)) 
  {
    // line[i][strlen(line[i]) - 1] = '\0';
    i++;
  }

  tot = i;

  printf("%i",i);

  int commands[tot][5];

  printf("\n The content of the file %s  are : \n",fname);    
  for(i = 0; i < tot; ++i)
  {
    for(j=0;j<5;j++){
          fscanf(fptr, "%i", &commands[i][j]);
          printf(" %i\n",commands[i][j]);
    }
    printf(" %s\n", line[i]);
  }
  printf("\n");

  return 0;
}