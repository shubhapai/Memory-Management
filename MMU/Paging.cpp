#include "util.h"
using namespace std;

/* Process flow of Virtual Page in memory */

void processVPage(int protectionBit, int vPage)
{
 
 /* Set Modified and Referenced bits */
   
  if (protectionBit == WRITE){
    pTable[vPage].modifiedBit=1;    
  }
  pTable[vPage].referencedBit=1;   
}

 /* Process of loading Virtual page to physical Frame */
 
void loadVPage(int protectionBit, int newVPage, int pFrame)
{
  
  if(pTable[newVPage].pagedoutBit==1)
  {

    if (Oflag){
	  /*  page in if new page has been pagedOut */
      printProcess("IN",newVPage,pFrame); 
    }

    //recording statistics
    stats.pageins++;
  }

  else
  {

    if (Oflag)
	{
      printProcess("ZERO",newVPage,pFrame); // If not its Zeroed
    }

    //recording statistics
    stats.zeros++;
  }

  if (Oflag)
  {
     /*  Map new Virtual page */
    printProcess("MAP",newVPage,pFrame); 
  }

  //recording statistics
  stats.maps++;

  //set Page Table values
  
  pTable[newVPage].presentBit=1;
  pTable[newVPage].referencedBit=1;    
  pTable[newVPage].numFrame=pFrame;
  
  if (protectionBit == WRITE)
  {
    pTable[newVPage].modifiedBit=1;    
  }

  fTable[pFrame]=newVPage;
}

/*  Process of removing page from current pFrame */

void removeVirtualPage(int pFrame)
{
  int curVPage=fTable[pFrame];
  pTable[curVPage].presentBit=0;
  pTable[curVPage].referencedBit=0;  
  pTable[curVPage].numFrame=0xfffffff;
  
  if (Oflag)
  {
  /* UNMAP the current Virtual Page */
    printProcess("UNMAP",curVPage,pFrame); 
  }

  //recording statistics
  stats.unmaps++;

  //if modifiedBit, pageOut
  if(pTable[curVPage].modifiedBit==1)
  {
    pTable[curVPage].modifiedBit=0;    
    pTable[curVPage].pagedoutBit=1;

    if (Oflag)
	{
	   /* Page out the current Virtual Page if it is modified */
      printProcess("OUT",curVPage,pFrame); 
    }

    //recording statistics
    stats.pageouts++;
  }

  fTable[pFrame]=-1;
}


