/*---------------------------------------------------------------------------
|
|              Tiny Lib - On/Off Controls (OutPins) - Nano versions
|
|  For small-RAM systems, the private stuff
|
-------------------------------------------------------------------------*/

#ifndef NANO_OUTPIN_PRIV_H
#define NANO_OUTPIN_PRIV_H

#include "common.h"
#include "nano_outpin.h"

#define _StateFlag   _BitM(0)
#define _LockedFlag  _BitM(1)

PUBLIC BIT nanoOutPin_Locked(S_OutPin CONST *p);
#define locked(p) nanoOutPin_Locked((p))

#endif // NANO_OUTPIN_PRIV_H

 
