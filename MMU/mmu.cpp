#include "PagereplacementAlgorithm.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

using namespace std;

struct STATISTICS stats;
int PTableSize=64;
struct PTE * pTable = new PTE[PTableSize];
int * fTable;
int FTableSize=32;

std::vector<int> randVals;
int rOfs=0;

bool Oflag=false;
bool Pflag=false;
bool fflag=false;
bool Sflag=false;

int PageUsedCount=0;
int randTotal=0;
int instTotal=0;
unsigned long long totalCost=0;

int main(int argc, char* argv[]) {
  char* algrthm=NULL;
  char* outInd=NULL;
  int c;

  while ((c = getopt (argc, argv, "a:f:o:")) != -1)
    switch (c){
      case 'a':
        algrthm = optarg;
        break;
      case 'f':
        FTableSize = atoi(optarg);
        break;
      case 'o':
        outInd = optarg;
        break;
      default:
        abort ();
    }

  PagereplacementAlgorithm *PageObject;
  /* If nothing specified use optimized solution */
  if (algrthm==NULL) PageObject = new LRU();
  else 
  if (strncmp(algrthm,"f",1)==0) 
     /* First In First Out */
		PageObject = new FIFO();
  else 
  if (strncmp(algrthm,"s",1)==0) 
     /* Derivative of FIFO */
		PageObject = new secondChance();
  else 
  if (strncmp(algrthm,"r",1)==0)
		PageObject = new Random();
  else 
  if (strncmp(algrthm,"N",1)==0)
       /* Not recently used */  
		PageObject = new NRU();
  else 
  if (strncmp(algrthm,"l",1)==0)
       /* Least recently used */
		PageObject = new LRU();
  else 
  if (strncmp(algrthm,"c",1)==0)
    /* Clock based on pFrames */       
       PageObject = new clockPhysical();
  else 
  if (strncmp(algrthm,"X",1)==0) 
    /*Clock based on Virtual Pages */
       PageObject = new clockVirtual();
  else 
  if (strncmp(algrthm,"a",1)==0) 
    /* Aging based on Physical Frames */
       PageObject = new agingPhysical();
  else 
  if (strncmp(algrthm,"Y",1)==0) 
    /* Aging based on Virtual Pages */
	   PageObject = new agingVirtual();

  //frame number
  initfTable(FTableSize);
  //stats
  initStat();
  //page table
  initpTable();

  int index=0;
  while (outInd[index]<='Z' && outInd[index]>='A')
  {
    c = outInd[index++];
    switch (c){
      case 'O':
        Oflag=true;
        break;
      case 'P':
        Pflag=true;
        break;
      case 'F':
        fflag=true;
        break;
      case 'S':
        Sflag=true;
        break;
      default:
        cout << "Unknown Option" << endl;
        abort();
    }
  }

  ifstream infile(argv[optind]);
  if (!infile) 
  {
    cout << "File not found. Please provide the correct file name"<<endl;
  }
  initRandomVals(argv[optind+1]);

  int protectionBit;
  int vPage;
  string line;

  while (!infile.eof()) 
  {
    getline(infile,line);
    if (line[0]=='#' || line.length()==0) 
	{
      continue;
    }        

	/* The 'O' (Ohhh) option prints output in reuired format*/
	
    if (Oflag){
      cout << "==> inst: " << line << endl;
    }
	
	/* get protectionBit and vPage from the instruction line */
	 
    istringstream iss(line);
    iss>>protectionBit>>vPage;

    instTotal ++;
    /* check if the Virtual Page is present in memory*/
    if (pTable[vPage].presentBit==1)
	{
      processVPage(protectionBit, vPage);
      PageObject->processVPage(vPage);
      continue;
    }

    //load the new page
    if (PageUsedCount<FTableSize)
	{
      loadVPage(protectionBit,vPage,PageUsedCount++);
      PageObject->loadVPage(vPage);
    }
	else
	{
      int pFrame = PageObject->mapPFrame();
      removeVirtualPage(pFrame);
      loadVPage(protectionBit,vPage,pFrame);
      PageObject->replaceVPage(vPage);
    }
  }

  if (Pflag)
  {
    /* pageTable option */
    printpTable();
  }

  if (fflag)
  {
    /* frameTable option */
    printfTable();
  }

  if (Sflag)
  {
    /* prints the summary line */
    printSummary();
  }
    
}

 void initpTable()
 {
  int i=0;
  for (;i<PTableSize;i++)
  {
    pTable[i].presentBit=0;
    pTable[i].modifiedBit=0;
    pTable[i].referencedBit=0;
    pTable[i].pagedoutBit=0;
    pTable[i].numFrame=0xfffffff;
  }
 }

void initfTable(int FTableSize)
{
  fTable = new int[FTableSize];
  for (int i=0;i<FTableSize;i++)
  {
    fTable[i]=-1;
  }
}

void initStat()
{
  stats.unmaps=0;
  stats.maps=0;
  stats.pageins=0;
  stats.pageouts=0;
  stats.zeros=0;
}

void initRandomVals(char * rfileName)
{
    //read the random value from random file
    ifstream rfile(rfileName);
    if (!rfile.is_open())
	{
        cout << "Error Opening "<<rfileName<<" , Please provide the correct file name" << endl;
        exit(0);
    }
    int random;
    rfile >>randTotal;
    while (!rfile.eof())
	{
      rfile >> random;
      randVals.push_back(random);
    }
}

int getRandomNumber(int r)
{
  if (rOfs == randTotal)
  {
    rOfs=0;
  }
  return (randVals[rOfs++] % r);
}

void printfTable()
{
  for (int i=0;i<FTableSize;i++)
  {
    if (fTable[i]==-1)
	{
      cout << "*";
    }
	else
	{
      cout << fTable[i];
    }
    cout << " ";
  }
  cout << endl;
}

void printpTable()
{
  int i=0;
  for (i;i<PTableSize;i++)
  {
    if (pTable[i].presentBit == 1)
	{
      cout << i << ":";
      char RefdModPagd[3];
      RefdModPagd[0] = pTable[i].referencedBit ? 'R' : '-';
      RefdModPagd[1] = pTable[i].modifiedBit ? 'M' : '-';
      RefdModPagd[2] = pTable[i].pagedoutBit ? 'S' : '-';
      cout << RefdModPagd[0] << RefdModPagd[1] << RefdModPagd[2];
    }
	else
	{
      if (pTable[i].pagedoutBit==1)
	  {
        cout << "#";
      }
	  else
	  {
        cout << "*";
      }
    }
    cout << " ";
  }
  cout << endl;
}

void printProcess(string process, int virtualPage, int pFrame)
{
  if (process == "ZERO")
  {
    cout << instTotal-1 << ": " << left << setw(5) << process << right << setw(8) << pFrame << endl;
  }
  else
  {
    cout << instTotal-1 << ": " << left << setw(5) << process << right << setw(4) << virtualPage << right << setw(4) <<pFrame << endl;
  }
}

void printSummary()
{
  totalCost+=instTotal;
  totalCost+=(stats.unmaps+stats.maps)*400;
  totalCost+=(stats.pageins+stats.pageouts)*3000;
  totalCost+=stats.zeros*150;
  printf("SUM %d U=%d M=%d I=%d O=%d Z=%d ===> %llu\n",
    instTotal, stats.unmaps, stats.maps, stats.pageins, stats.pageouts, stats.zeros, totalCost);
}





