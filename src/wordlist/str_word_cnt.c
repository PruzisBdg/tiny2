/*---------------------------------------------------------------------------
|
|                    Space-delimited Word lists
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "wordlist.h"

/*-----------------------------------------------------------------------------------------
|
|  Str_WordCnt
|
|  Returns the number of words in 'str'
|
------------------------------------------------------------------------------------------*/

PUBLIC U8 Str_WordCnt( U8 GENERIC const *str )
{
   U8 rIDATA cnt, ch;
   BIT spc;

   spc = 1;                            // Assume (leading) spaces until get char
   for( cnt = 0;; str++ )              // for each char in str
   {
      ch = *str;
      
      if( ch == 0 ) { return cnt; }    // end of 'str'? return cnt.
   
      if( spc )                        // currently parsing spaces? 
      {
         if( !Str_Delimiter(ch) )      // now got a char?
         {
         spc = 0;                       
         cnt++;                        // then its another word
         }
      }
      else                             // else currently parsing chars
      {
         if( Str_Delimiter(ch) ) { spc = 1; }  // got space? flip to parsing spaces
      }   
   }
}


// --------------------------------------- eof -----------------------------------------
