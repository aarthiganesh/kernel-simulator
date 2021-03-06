# Operating Systems: Process Scheduling, Memory Management
## Compiling the project
To compile the code using `gcc` use the following command:

`gcc assignment2.c -o assignment2`

## Running the project
`./assignment2 input.txt output.txt [command]`

where [command] can be replaced by one of the following commands:
* `fcfs` - first come first serve scheduling
* `fcfsIO` - first come first serve scheduling with I/O bursts
* `priority` - priority scheduling 
* `rr` - round robin scheduling
* `mm1` - memory management using partition arrangement 1
  1- 500 Mb
  2- 250 Mb
  3- 150 Mb
  4- 100 Mb 
* `mm2` - memory management using partition arrangement 2
  1- 350 Mb
  2- 300 Mb
  3- 300 Mb
  4- 50 Mb 

`input.txt` is the input file that holds the list of processes and  `output.txt` is the file where the series of event changes are recorded.

# Project Objectives
The objective of this assignment is to build a small simulator of an OS kernel, which could be used for performance analysis of different scheduling algorithms. In this portion of the assignment we will be analysing 3 different scheduler types and using memory management techniques. 

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

## Assumptions about memory management
* Partitions will be listed out in order from largest to smallest space
* ALL THE PROCESSES IN THE INPUT FILE WILL NOT HAVE A MEMORY REQUIREMENT LARGER THAN THE SIZE OF THE BIGGEST PARTITION
* FCFS and Best Fit method will be used to determine which partition will be used for the incoming processes

when a process starts, it will demand the memory needed; if not available, the process cannot start
and must wait until memory is available. The simulator must record, every time a new process starts:
  a. Total amount of memory used
  b. Partitions used/free
  c. Total amount of free memory available
  d. Amount of “usable” memory available (i.e., total amount of memory available in partitions; DO
NOT COUNT the internal fragmentation, as those fragments are not usable). 
## Input Data

`| PID | Priority | Arrival Time | Total CPU Time | IO Frequency | IO Duration | Memory`

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
- Memory: amount of memory that needs to be allocated for the specified process

## Output File
* The time in all of the output files will be indicated in seconds EXCEPT FOR IN THE ROUND ROBIN OUTPUT where it will be indicated in milliseconds

### FCFS and FCFS IO
`Time(s) | Process ID | Old State | New State`
### Priority
`Time(s) | Process ID | Priority | Old State | New State`
### Round Robin
`Time(ms) | Process ID | Old State | New State`
### Memory Management
`Time(s) | Process ID | Old State | New State | P1AVAIL | P2AVAIL | P3AVAIL | P4AVAIL | Used MEM | Available MEM | Usable MEM `

Where `P1AVAIL` indicates the availatility of partition 1