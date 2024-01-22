#pragma once
#include <vector>
#include <Windows.h>
#include "proc.h"


class Offsets
{
public:
  
  
  Offsets(HANDLE handleProc, uintptr_t gameAssemblyBase) : gaBase(gameAssemblyBase), hProc(handleProc)
  {
    dynamicCommonOrbAddr += gaBase;
    dynamicCashOpCodeAddr += gaBase;
    dynamicHealthOpCodeAddr += gaBase;
    dynamicEssenceOpCodeAddr += gaBase;
    dynamicCrystalOpCodeAddr += gaBase;
    resolveOrbs();
  }
  //orbs
  uintptr_t commonOrbAddr = 0;
  uintptr_t uncommonOrbAddr = 0;
  uintptr_t rareOrbAddr = 0;
  uintptr_t superRareOrbAddr = 0;
  uintptr_t epicOrbAddr = 0;

  //cash
  uintptr_t dynamicCommonOrbAddr = 0x1d84f58; //will be changed immediately after the object is constructed
  uintptr_t dynamicCashOpCodeAddr = 0x611c56; // change to mulsd from movaps for much more money
  uintptr_t dynamicHealthOpCodeAddr = 0x614465; // nop the call to disable the health decreasing
  uintptr_t dynamicEssenceOpCodeAddr = 0x33d8c3; // nop the call to disable the cosmic essence from decrementing
  uintptr_t dynamicCrystalOpCodeAddr = 0x3d7e1c; //change call to mov eax,04234584 for a lot of shards when going into martian games

private:
  void resolveOrbs();
  uintptr_t gaBase;
  HANDLE hProc;
  
  //Orbs
  const std::vector<unsigned int> commonOrbOffsets = { 0x68,0xc60,0x30,0x18,0x18,0x2c };


};


