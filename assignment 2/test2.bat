:: Runs Assignment 2 with test input.txt
:: Output files are stored in output.txt

gcc assignment2.c -o assignment2
./assignment2 ./testcases/test3.txt output.txt fcfs
./assignment2 ./testcases/test3.txt output.txt fcfsIO
./assignment2 ./testcases/test3.txt output.txt priority
./assignment2 ./testcases/test3.txt output.txt rr

./assignment2 ./testcases/test3.txt output.txt mm1
./assignment2 ./testcases/test3.txt output.txt mm2