/*---------------------------------------------------------------------------
|
|                  An Unstructured single-block Flash Store
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "flash_basic.h"
#include "util.h"
#include "arith.h"
#include "flashblock.h"

extern S_FlashBlock RAM_IS BasicFlashBlock;

#define B BasicFlashBlock

/*-----------------------------------------------------------------------------------
|
|  FlashBlock_WriteAll()
|
|  Write the whole shadow back to Flash.
|
--------------------------------------------------------------------------------------*/

PUBLIC BIT FlashBlock_WriteAll(void)
{
   if( !Flash_Erase(B.addr, 1) )    // Erase Flash block?
      { return 0; }                 // Erase failed, return 0   
   else                             // and rewrite with revised shadow.
      { return Flash_WrBlock(B.shadow, B.addr, B.size); }    // return 1 if success, o if fail
}


// ----------------------------- eof ---------------------------------------
 
