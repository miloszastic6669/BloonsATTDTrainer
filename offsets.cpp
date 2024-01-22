#include "offsets.h"

void Offsets::resolveOrbs()
{
  commonOrbAddr = FindDMAAddy(hProc, dynamicCommonOrbAddr, commonOrbOffsets);
  uncommonOrbAddr = commonOrbAddr + 0x10;
  rareOrbAddr = commonOrbAddr + 0x20;
  superRareOrbAddr = commonOrbAddr + 0x30;
  epicOrbAddr = commonOrbAddr + 0x40;
}
