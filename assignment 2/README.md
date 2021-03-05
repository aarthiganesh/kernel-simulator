# Operating Systems: Process Scheduling, Memory Management
## Compiling the project
To compile the code using `gcc` use the following command:

`gcc assignment2.c -o assignment1`

## Running the project
`./assignment2 input.txt output.txt`

where `input.txt` is the input file that holds the list of processes and  `output.txt` is the file where the series of event changes are recorded.

# Project Objectives
The objective of this assignment is to build a small simulator of an OS kernel, which could be used for performance analysis of different scheduling algorithms.

## Assumptions Made

### Assume that transitions take zero time
(that is, do not worry for the time taken by the ISRs, scheduler, context switch, etc.). 
Every time the simulation changes states, the following information is included in an output file.
* Time: The simulation time starts at 0, THE SIMULATION TIME IS MEASURED USING A COUNTER
* Pid: the id for the process that has changed state
* Old State/New State: The state of the process before and after the transition
* Partition

NOTE: since the transitions take 0 time, a process that is READY can switch to RUNNING as soon as the thread becomes available

### Assume an IO will always be available
That is, do not worry about waiting queues at the I/O devices: whenever you request an I/O, the I/O starts immediately).

## Input Data

The input data will be included in an input.txt file. Each line corresponds to a different process, and each value in the line corresponds to the following values:
- Pid: a unique identifier for the process
- Priority: the priority value of the process, the higher the number, the higher the priority
- Arrival Time: the initial simulated time is 0 and the arrival time can be at 0 or any time value
thereafter. The time units to be used are milliseconds.
- Total CPU Time: it is the total time the process needs to complete (does not include I/O time: only
the total time needed in the CPU)
- I/O Frequency: the processes are assumed to make an input/output with this frequency
- I/O duration: this is the duration for the I/O for each of the processes (assumed to be the same for
all the I/O operations)
