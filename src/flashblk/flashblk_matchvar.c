/*---------------------------------------------------------------------------
|
|        Test Fixture Support - Fixed-size block Flash storage
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "flashblk.h"

extern T_FlashAddr getVarAddr_flashBlk( T_FlashBlk_Handle hdl );
extern T_FlashAddr bankEnd_flashBlk( void );
#define bankEnd bankEnd_flashBlk


#define _SlotDataBytes  2                                               // block holds an integer
#define _SlotSize       (_SlotDataBytes + sizeof(T_FlashBlk_Handle))    // data bytes plus handle


/*-----------------------------------------------------------------------------------
|
|  FlashBlk_MatchVar()
|
|  Return 1 if the store has a variable labelled 'hdl' whose value matches 'src'.
|  'src' is 'cnt' bytes in size.
|
|  Note: To check just that a variable exists set cnt to 0.
|
|  PARAMETERS:    'src'    - address of data to be compared
|                 'cnt'    - number of bytes from 'src' to be compared
|                 'hdl'    - the handle a the data to be compared
|
|  RETURNS:       1 if a match, else 0
|
|	DATE        AUTHOR            DESCRIPTION OF CHANGE 
|	--------		-------------		-------------------------------------
|
--------------------------------------------------------------------------------------*/

PUBLIC U8 FlashBlk_MatchVar( U8 *src, T_FlashBlk_Handle hdl, U8 cnt )
{
   T_FlashAddr p;
   U8          c;
   
   if( !(p = getVarAddr_flashBlk(hdl)) )        // 'hdl' not found in store?
      { return 0; }                             // then no match

   else                                         // else match for 'hdl'. Assume store data is contiguous, compare it against 'src'
   {
      for( ; p < bankEnd(); )
      {
         p++;                                   // Advance from handle to 1st data byte of this slot

         for( c = _SlotDataBytes; c; c-- )      // For each data byte in the slot
         {
            if( !cnt )                          // compared all 'src' bytes (no mismatch found)?
               { return 1; }                    // then return a match
            else                                // else more bytes to compare
            {
               if( FlashBlk_GetCh(p) != *src )     // store byte doesn't match 'src'?
                  { return 0; }                 // then return a mismatch
               else                             // else a match
               {
                  p++; src++, cnt--;            // so advance pointers, decrement count and compare next byte
               }
            }
         }
         if( cnt && (FlashBlk_GetHdl(p) != hdl) )   // bytes to compare yet, but now handles don't match?
            { return 0; }                       // then mismatch
      }
      return 0;         // reached bank end before all bytes were compared.. a mismatch
   }
}

// ----------------------------------------- eof -------------------------------------- 
