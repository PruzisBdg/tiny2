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
|  FlashBlock_Write()
|
|  Write up to 'cnt' bytes from 'src', starting at 'ofs'
|
--------------------------------------------------------------------------------------*/

PUBLIC BIT FlashBlock_Write(U8 RAM_IS *src, U8 ofs, U8 cnt)
{
   if( ofs >= B.size )                 // Start address is past end of block?
   {
      return 0;                        // then fail
   }
   else                                // else start address is within block
   {                                   // So copy entire block into shadow
      CopyConstBytesU8( (U8 CODE *)B.addr, (U8 RAM_IS *)B.shadow, B.size);
                                       // and update shadow with new data
      CopyBytesU8( src, B.shadow + ofs, MinU8(cnt, B.size - ofs));

      return FlashBlock_WriteAll();
   }
}


// ----------------------------- eof ---------------------------------------

