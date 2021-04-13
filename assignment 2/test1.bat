:: Runs Assignment 2 with test input.txt
:: Output files are stored in output.txt

gcc assignment2.c -o assignment2
./assignment2 input.txt output.txt fcfs
./assignment2 input.txt output.txt fcfsIO
./assignment2 input.txt output.txt priority
./assignment2 input.txt output.txt rr

./assignment2 input.txt output.txt mm1
./assignment2 input.txt output.txt mm2