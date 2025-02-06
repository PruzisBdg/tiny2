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
|  FlashBlock_Read()
|
|  Read 'cnt' bytes to 'out', starting at 'ofs'. If the read extends beyond the
|  end of the Flash block, then the extra bytes are undefined.
|
|  Returns the number of bytes read.
|
--------------------------------------------------------------------------------------*/

PUBLIC U8 FlashBlock_Read(U8 RAM_IS *out, U8 ofs, U8 cnt)
{
   CopyConstBytesU8( (U8 CODE *)(B.addr + ofs), out, cnt);
   return cnt;
}

// ----------------------------- eof ---------------------------------------
 
