/*-----------------------------------------------------------------------------------
|
|  FlashBlk_VarExists()
|
|  Search for a variable labelled 'hdl' in the store.
|
|  PARAMETERS:    'hdl'    - the handle to find
|
|  RETURNS:       1 if 'hdl' exists, else 0
|
|	DATE        AUTHOR            DESCRIPTION OF CHANGE 
|	--------		-------------		-------------------------------------
|
--------------------------------------------------------------------------------------*/

#include "flashblk.h"

extern T_FlashAddr getVarAddr_flashBlk( T_FlashBlk_Handle hdl );

PUBLIC U8 FlashBlk_VarExists( T_FlashBlk_Handle hdl )
{
   return getVarAddr_flashBlk(hdl) != 0;
}

// -------------------------------- eof --------------------------------------------- 
