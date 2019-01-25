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
|  OutPin_Init()
|
|  Make S_OutPin 'obj' from 'init' and reset it. Always returns 1 (success)
|
-----------------------------------------------------------------------------------*/

PUBLIC U8 OutPin_Init( T_ObjAddr obj, T_CodeAddr init )
{
   *((S_OutPin CONST *)init)->flags = 0;
   OutPin_UpdatePin((S_OutPin CONST *)init, ((S_OutPin CONST*)init)->resetState);
   return 1;
}


// --------------------------------- eof -------------------------------------------






 
