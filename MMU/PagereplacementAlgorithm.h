#include "util.h"

class PagereplacementAlgorithm 
{
  public:
  PagereplacementAlgorithm(){}
  virtual void processVPage(int vPage) = 0;
  virtual void loadVPage(int vPage) = 0;
  virtual void replaceVPage(int vPage) = 0;
  virtual int mapPFrame() = 0; 
};


class FIFO : public PagereplacementAlgorithm 
{
  private:
  int rCOUNT; // replacement count
  
  public:
  FIFO()
  {
    rCOUNT=0;
  }
    void processVPage(int vPage){}
    void replaceVPage(int vPage){}
    void loadVPage(int vPage){}

    int mapPFrame()
	{
      return rCOUNT++ % FTableSize;
    }
	
};

class secondChance : public PagereplacementAlgorithm 
{
  private:
    int rCOUNT;
  
  public:
    secondChance()
	{
      rCOUNT=0;
    }

    void processVPage(int vPage){}
    void replaceVPage(int vPage){}
    void loadVPage(int vPage){}

    int mapPFrame(){
      int mapped=rCOUNT++ % FTableSize;
      while (pTable[fTable[mapped]].referencedBit==1)
	  {
        pTable[fTable[mapped]].referencedBit=0;
        mapped=rCOUNT++ % FTableSize;
      }
      return mapped;
    }
	
};

class Random : public PagereplacementAlgorithm 
{
  public:
    void processVPage(int vPage){}
    void loadVPage(int vPage){}
    void replaceVPage(int vPage){}

    int mapPFrame()
	{
      return getRandomNumber(FTableSize);
    }
	
};


class NRU : public PagereplacementAlgorithm {
  private:
  int length;      
  int rCOUNT;
  int * NotRecentUsedPtr;


  public:
    NRU()
	{
	  length=0;
      rCOUNT=1;
      NotRecentUsedPtr = new int[PTableSize];
    }

    void processVPage(int vPage){}
    void loadVPage(int vPage){}
    void replaceVPage(int vPage)
	{
      rCOUNT++;
    }

    int mapPFrame()
	{
      getLowestPTEs();
      clearReferenced();
	  
      int rand=getRandomNumber(length);
      int vPage=NotRecentUsedPtr[rand];
      return pTable[vPage].numFrame;
    }

    void getLowestPTEs()
	{
      length=0;
	  
      int minValue=INT_MAX; // Maximum value for an object of type int
      /* scan physical frames */ 
      
      for (int i=0;i<PTableSize;i++)
	  {
        if (pTable[i].presentBit==0)
		{
          continue;
        }

        if (countValue(i)<minValue)
		{
          minValue=countValue(i);
          length=0;
          NotRecentUsedPtr[length++]=i;
        }
		else if (countValue(i)==minValue)
		{
          NotRecentUsedPtr[length++]=i;
        }
      }
    }
 
    int countValue(int ind)
	{
      int referenceBitFlag = pTable[ind].referencedBit;
      int modifiedBitFlag = pTable[ind].modifiedBit;
      return 2*referenceBitFlag+modifiedBitFlag;
    }

    void clearReferenced()
	{
      
      if (rCOUNT%10 == 0)
	  {
        for (int i=0;i<PTableSize;i++)
		{
          pTable[i].referencedBit=0;
        }
      }
    }
};

class LRU : public PagereplacementAlgorithm 
{
  private:
  /* List of Frames */
  std::list<int> fList;
  
  public:
  LRU(){}

    void processVPage(int vPage)
	{
      int numFrame = pTable[vPage].numFrame;
      for (std::list<int>::iterator it=fList.begin(); it!=fList.end(); ++it)
	  {
        if ((*it)==numFrame) 
		{
          fList.erase(it);
          break;
        }
      }
      fList.push_front(numFrame);
    }
    
    void replaceVPage(int vPage)
	{
      fList.pop_back();
      fList.push_front(pTable[vPage].numFrame);
    }

    void loadVPage(int vPage)
	{
      fList.push_front(pTable[vPage].numFrame);
    }

    int mapPFrame()
	{
      int i=fList.back();
      return i;
    }
	
};

class clockPhysical : public PagereplacementAlgorithm 
{
  private:
    int clockPtr;
  
  public:
    clockPhysical()
	{
        clockPtr=0;
    }

    void processVPage(int vPage){}
    void replaceVPage(int vPage){}
    void loadVPage(int vPage){}

    int mapPFrame()
	{
      int mapped=clockPtr++ % FTableSize;
      while (pTable[fTable[mapped]].referencedBit==1)
	  {
        pTable[fTable[mapped]].referencedBit=0;
        mapped=clockPtr++ % FTableSize;
      }
      return mapped;
    }
};

class clockVirtual : public PagereplacementAlgorithm {
  private:
    int clockPtr;
  
  public:
    clockVirtual()
	{
      clockPtr=0;
    }

    void processVPage(int vPage){}
    void replaceVPage(int vPage){}
    void loadVPage(int vPage){}

    int mapPFrame(){
      int mapped=clockPtr++ % PTableSize;
      while (pTable[mapped].presentBit==0 || pTable[mapped].referencedBit==1)
	  {
        pTable[mapped].referencedBit=0;
        mapped=clockPtr++ % PTableSize;
      }
      return pTable[mapped].numFrame;
    }
};


class agingPhysical : public PagereplacementAlgorithm 
{
  private:
    unsigned int * agePtr;
  
  public:
    agingPhysical()
	{
      agePtr = new unsigned int[FTableSize];
      for (int i=0; i<FTableSize; i++) 
	  {
        agePtr[i]=0;
      }
    }

    void processVPage(int vPage){}
    void replaceVPage(int vPage){}
    void loadVPage(int vPage){}

    int mapPFrame()
	{
      for (int i=0; i<FTableSize; i++) 
	  {
        agePtr[i]=agePtr[i]>>1;
        if (pTable[fTable[i]].referencedBit)
		{
          agePtr[i]=agePtr[i]|0x80000000;
          pTable[fTable[i]].referencedBit=0;
        }
      }
      int tempframe=0;
      for (int i=1; i<FTableSize; i++)
	  {
        if (agePtr[i]<agePtr[tempframe]) 
		{
          tempframe=i;
        }
      }
        agePtr[tempframe]=0;
      return tempframe;
    }
};

class agingVirtual : public PagereplacementAlgorithm 
{
  private:
    unsigned int * agePtr;
  
  public:
    agingVirtual()
	{
      agePtr = new unsigned int[PTableSize];
      for (int i=0; i<PTableSize; i++) 
	  {
        agePtr[i]=0;
      }
    }

    void processVPage(int vPage){}
    void replaceVPage(int vPage){}
    void loadVPage(int vPage){}

    int mapPFrame()
	{
      for (int i=0; i<PTableSize; i++) 
	  {
        agePtr[i]=agePtr[i]>>1;
        if (pTable[i].referencedBit)
		{
          agePtr[i]=agePtr[i]|0x80000000;
          pTable[i].referencedBit=0;
        }
      }
      int tempPage=0;
      while (!pTable[tempPage].presentBit)
	  {
        tempPage++;
      }
      for (int i=1; i<PTableSize; i++) 
	  {
        if (pTable[i].presentBit==1 && agePtr[i]<agePtr[tempPage]) 
		{
          tempPage=i;
        }
      }
        agePtr[tempPage]=0;
      return pTable[tempPage].numFrame;
    }    
};

