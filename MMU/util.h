#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <list>
#include <climits>
#include <cstring>
#include <iomanip>

#define READ 0
#define WRITE 1

/* Paging statistics */

struct STATISTICS {
  int unmaps;
  int maps;
  int pageins;
  int pageouts;
  int zeros;
};

/* Bit fields Structure of Page table entry*/

struct PTE {
  unsigned int presentBit:1;
  unsigned int modifiedBit:1;
  unsigned int referencedBit:1;
  unsigned int pagedoutBit:1;
  unsigned int numFrame:28;
};


extern struct STATISTICS stats;
extern struct PTE * pTable;
extern int PTableSize; // Size of page Table
extern int * fTable;
extern int FTableSize; // Size of frame Table

extern bool Oflag;
extern bool Pflag;
extern bool Fflag;
extern bool Sflag;

extern int instTotal;
extern unsigned long long totalCost;

extern std::vector<int> randVals;
extern int rOfs;
extern int rTotal;
int getRandomNumber(int r);
void initRandomVals(char* rfileName);

void initStat();
void initpTable();
void initfTable(int FTableSize);

void processVPage(int protectionBit, int pFrame);
void removeVirtualPage(int pFrame);
void loadVPage(int protectionBit, int newVPage, int pFrame);

void printProcess(std::string operation, int virtualPage, int physicalFrame);
void printpTable();
void printfTable();
void printSummary();

