/*---------------------------------------------------------------------------
|
|        Test Fixture Support - Fixed-size block Flash storage
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "flashblk.h"

extern T_FlashAddr currentBank_flashBlk( void );

#define _SlotDataBytes  2                                               // block holds an integer
#define _SlotSize       (_SlotDataBytes + sizeof(T_FlashBlk_Handle))    // data bytes plus handle


/*-----------------------------------------------------------------------------------
|
|  FlashBlk_Delete()
|
|  Delete item 'hdl' from the store, if it exists
|
|  PARAMETERS:
|
|  RETURNS:
|
|	DATE        AUTHOR            DESCRIPTION OF CHANGE 
|	--------		-------------		-------------------------------------
|
--------------------------------------------------------------------------------------*/

PUBLIC void FlashBlk_Delete( T_FlashBlk_Handle hdl )
{
   T_FlashAddr p;

   Flash_SetBank(FlashBlk_GetCodeBank());

   for( p = currentBank_flashBlk(); FlashBlk_GetHdl(p) != 0xFF; p += _SlotSize )  // for each used block
   {
      if( FlashBlk_GetHdl(p) == hdl )                                    // if it handle matches variable to be written          
      {
         Flash_WrByte( p, 0x00 );                                    // then mark block as deleted   
      }
   }
}

// -------------------------------- eof ------------------------------------- 
