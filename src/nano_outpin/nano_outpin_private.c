/*---------------------------------------------------------------------------
|
|                       Tiny Lib - On/Off Controls (Actuators)
|
-------------------------------------------------------------------------*/

#include "common.h"
#include "nano_outpin.h"
#include "nano_outpin_priv.h"

PUBLIC BIT nanoOutPin_Locked(S_OutPin CONST *p) { return BSET(*(p->flags),_LockedFlag); }

// --------------------------------- eof -------------------------------------------






 
