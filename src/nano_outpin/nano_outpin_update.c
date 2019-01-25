/*---------------------------------------------------------------------------
|
|                       Tiny Lib - On/Off Controls (Actuators)
|
-------------------------------------------------------------------------*/

#include "common.h"
#include "nano_outpin.h"
#include "nano_outpin_priv.h"

/*--------------------------------------------------------------------------------
|
|  OutPin_UpdatePin()
|
|  Set 'p' from 'n'. 'p' is on (set) if n <> 0, else it is off (clear)
|
-----------------------------------------------------------------------------------*/

PUBLIC void OutPin_UpdatePin(S_OutPin CONST *p, U8 n)
{
   *(p->flags) = (*(p->flags) & ~_StateFlag) | ((n != 0) ? _StateFlag : 0);
   
   if( p->wrPin )                          // there's a write function? (there should be!!)
      { (*p->wrPin)(BSET(*(p->flags),_StateFlag)); }           // then run it.
}

// --------------------------------- eof -------------------------------------------






 
