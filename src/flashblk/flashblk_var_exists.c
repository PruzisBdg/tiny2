/*-----------------------------------------------------------------------------------
|
|  FlashBlk_VarExists()
|
|  Search for a variable labelled 'hdl' in the store.
|
--------------------------------------------------------------------------------------*/

#include "flashblk.h"

extern T_FlashAddr getVarAddr_flashBlk( T_FlashBlk_Handle hdl );

PUBLIC U8 FlashBlk_VarExists( T_FlashBlk_Handle hdl )
{
   return getVarAddr_flashBlk(hdl) != 0;
}

// -------------------------------- eof --------------------------------------------- 
