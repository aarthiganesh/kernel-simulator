# SYSC 4001 Assignment 3: Concurrency, IPC, Semaphores, Shared Memory, Virtual Memory, Files

## PART A: Execution Instructions

Terminal 1: `gcc process1.c -o process1` 

Terminal 2: `gcc process2.c -o process2`  **NOTE: THE COMPILED FILE MUST BE NAMED process2

All the terminal will then prompt the user to enter either `1`, `2` or `x`. (`x` will terminate all of the processes)

## PART B: ATM execution instructions
There are 3 different files that are required to properly run the ATM. There is ATM.c, DBSERVER.c and DBEDITOR. As shown in the assignment system diagram the ATM will ask the user to enter uinique identification information in order to access an account. The DBSERVER will find relevant information and send it back to the ATM and DBEDITOR will update the database file (db.txt) when any changes occur to the database.

### db.txt Format
The format of db.txt MUST BE AS FOLLOWS

```<account number> <pin> <balance>```

An example of the db.txt file is as follows:

```00001 107 3512.43
00011 323 10292.78
00117 259 -102.00```

### STEP 1: Check if there is any existing queue information that could interfere with the program and delete them
Sometimes when running the program multiple time after hard stops using `CTRL + C` the queues created using msgget are not killed and interfere with the second run. 

To search for the queues use 
`ipcs -q`

and to delete the queues use
`ipcrm -q<qid>`

### STEP 2: Compile all three c programs in different terminal windows
Terminal 1: `gcc ATM.c -o atm`

Terminal 2: `gcc DBSERVER.c -o server`

Terminal 3: `gcc DBEDITOR.c -o editor`

### STEP 3: the c programs should then be run using the following commands

Terminal 1: `./atm`

Terminal 2: `./server`

Terminal 3: `./editor`

## Prompts

You will be prompted by the ATM to first enter a PIN and an ACCOUNT NUMBER (it is assumed that the account number will be valid, please enter a valid account number)
--> if you enter a wrong pin 3 times then you will be blocked from the account

Sample value used for testing: `PIN: 107 Account Number: 00001`

### BANKING OPERATIONS
The available banking operations are:
`balance` : this will show the account balance
and
`withdraw` : this will allow the user to withdraw money if they have sufficient funds in the account

## ASSIGNMENT NOTE:
One of the drawbacks of this implementation is that it is unable to read the correct account balances from the original file, however the balance manipulation will be exact. 