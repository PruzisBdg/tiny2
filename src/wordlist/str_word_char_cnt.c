/*---------------------------------------------------------------------------
|
|                    Space-delimited Word lists
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "wordlist.h"


/*-----------------------------------------------------------------------------------------
|
|  Str_WordCharCnt()
|
|  Returns the number of chars in the next word. Bypasses leading whitespace
|  
------------------------------------------------------------------------------------------*/

PUBLIC U8 Str_WordCharCnt( U8 GENERIC *w )
{
   U8 ch, cnt;
   BIT inWord;
   
   for( cnt = 0, inWord = 0;; w++ )
   {
      ch = *w;
      
      if( ch == '\0' )                 // end of string
         { return cnt; }               // then return count
         
      if(!inWord)                      // haven't reach 1st word?
      {
         if(!Str_Delimiter(ch))        // but reached it now?
         {
            inWord = 1;                // mark the we reached an word
            cnt = 1;                   // and have its 1st char
         }
      }
      else                             // else already counting a word
      {
         if( Str_Delimiter(ch) )       // end of word?
            { return cnt; }            // return char count
         else                          // else another char
            { cnt++;  }                // which we count
      }
   }
}

// ------------------------------- eof ------------------------------------
 
