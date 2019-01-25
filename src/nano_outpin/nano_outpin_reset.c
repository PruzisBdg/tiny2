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
|  OutPin_Reset()
|
|  If 'p' is not locked then reset it. Returns 0 if 'p' could not be reset.
|
-----------------------------------------------------------------------------------*/

PUBLIC BIT OutPin_Reset( S_OutPin CONST *p )
{
   if( locked(p) )
   {
      return 0;
   }
   else
   {
      OutPin_UpdatePin(p, 0);
      return 1;
   }
}

// --------------------------------- eof -------------------------------------------






 
