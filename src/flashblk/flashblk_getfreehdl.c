/*---------------------------------------------------------------------------
|
|        Test Fixture Support - Fixed-size block Flash storage
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "flashblk.h"

extern T_FlashAddr getVarAddr_flashBlk( T_FlashBlk_Handle hdl );

/*-----------------------------------------------------------------------------------
|
|  FlashBlk_GetFreeHdl()
|
|  Find the lowest unused handle which is in the range 'min' - 'max'
|
|  PARAMETERS:    'min'    - low end of the handle range to search
|                 'max'    - top end "  "
|
|  RETURNS:  the free handle, else 0 if non found
|
|	DATE        AUTHOR            DESCRIPTION OF CHANGE 
|	--------		-------------		-------------------------------------
|
--------------------------------------------------------------------------------------*/

PUBLIC T_FlashBlk_Handle FlashBlk_GetFreeHdl(T_FlashBlk_Handle min, T_FlashBlk_Handle max )
{
   T_FlashBlk_Handle hdl;
   
   for( hdl = min; hdl <= max; hdl++ )    // from the lowest user handle upwards
   {
      if( !getVarAddr_flashBlk(hdl) )     // there's no variable with that handle?
      { 
         return hdl;                      // then we can use it.
      }                  
   }  
   return 0;                              // else all handles were spoken for
}


// --------------------------------- eof ---------------------------------------- 
