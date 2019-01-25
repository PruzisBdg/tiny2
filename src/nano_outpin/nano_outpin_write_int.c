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
|  OutPin_WriteInt()
|
|  If 'p' is not locked then set its state from 'n'. If 'n' == 0 then the pin is off
|  (clear) else its is on (set)
|
|  Same as OutPin_Write() but follows the call model for Class write functions
|
-----------------------------------------------------------------------------------*/

PUBLIC void OutPin_WriteInt( S_OutPin CONST *p, S16 n ) { OutPin_Write(p, (U8)n); }


// --------------------------------- eof -------------------------------------------






 
