/*---------------------------------------------------------------------------
|
|        Test Fixture Support - Fixed-size block Flash storage
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "flashblk.h"

extern T_FlashAddr currentBank_flashBlk( void );
extern T_FlashAddr bankEnd_flashBlk( void );

#define _SlotDataBytes  2                                               // block holds an integer
#define _SlotSize       (_SlotDataBytes + sizeof(T_FlashBlk_Handle))    // data bytes plus handle


/*-----------------------------------------------------------------------------------
|
|  FlashBlk_Delete()
|
|  Delete item 'hdl' from the store, if it exists
|
--------------------------------------------------------------------------------------*/

PUBLIC void FlashBlk_Delete( T_FlashBlk_Handle hdl )
{
   T_FlashAddr p, end;
   T_FlashBlk_Handle h;

   Flash_SetBank(FlashBlk_GetCodeBank());

   for( p = currentBank_flashBlk(), end = bankEnd_flashBlk();
        p <= end; p += _SlotSize )                             // for each used block
   {
      if( (h = FlashBlk_GetHdl(p)) == 0xFF)                    // Reached unprogrammed area (without finding 'hdl')?
      {
         return;                                               // then 'hdl' is not in store; we are done.
      }
      else if( h == hdl )                                      // else matched 'hdl'
      {
         Flash_WrByte( p, 0x00 );                              // then mark block as deleted
         return;
      }
   }
}

// -------------------------------- eof ------------------------------------- 
