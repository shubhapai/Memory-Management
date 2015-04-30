CC=g++
all: mmu

mmu: mmu.o Paging.o
	$(CC) mmu.o Paging.o -o mmu -std=c++0x

Paging.o: Paging.cpp
	$(CC) -c Paging.cpp -std=c++0x
	
mmu.o: mmu.cpp
	$(CC) -c mmu.cpp -std=c++0x

clean:
	rm -rf *o mmu
