# Memory-Management

 Follow the steps to test the program

 1. Run makefile to get mmu executable
 2. Run the script  ./runit_genclass.sh ./outputs ./mmu to generate the output files.
 3. Run the script ./gradeit.sh ./outputs ./myoutputs 

The input format for this program will be a sequence of “instructions” and optional comment line (shown below). A
comment line anywhere in the input file starts with ‘#’ is completely ignored by program and not
count towards the instruction count. An instruction line comprises of two integers, where the first number
indicates whether the instruction is a read (value=0) or a write (value=1) operation and which virtual page is
touched by that operation.

#page reference generator
#inst=1000000 pages=64 %read=75.000000 lambda=2.000000
1 13
0 34
0 18




