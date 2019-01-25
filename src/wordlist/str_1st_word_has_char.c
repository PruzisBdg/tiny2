/*---------------------------------------------------------------------------
|
|                    Space-delimited Word lists
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "wordlist.h"

/*-----------------------------------------------------------------------------------------
|
|  Str_1stWordHasChar()
|
|  If the 1st word in 'lst' contains 'ch' then return an index to 'ch', RELATIVE TO
|  THE START OF 'LST' (not relative to the start of word). Return _Str_NoMatch if
|  'ch' not found.
|
------------------------------------------------------------------------------------------*/

PRIVATE BIT charEndsWord(U8 ch)
{
   return Str_Delimiter(ch) || ch == '\0';
}

PUBLIC U8 Str_1stWordHasChar( U8 GENERIC const *lst, U8 ch )
{
   U8 c;
   
   for(c = 0; ; c++)                            // Walk thru 'lst'
   {
      if(lst[c] == ch)                          // Matched 'ch'?
         { return c; }                          // then return index to match
      if( c >= 0xFE || charEndsWord(lst[c]) )   // Word is too long OR got word-delimiter?
         { return _Str_NoMatch; }               // the return failed to match.
   }
}

// -------------------------------- eof -------------------------------------  
