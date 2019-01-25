/*---------------------------------------------------------------------------
|
|                    Space-delimited Word lists
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "wordlist.h"

/*-----------------------------------------------------------------------------------------
|
|  Str_GetNthWord()
|
|  Return pointer to the 'n'th word in the space-delimited word list 'lst'. If there 
|  aren't enough words, returns ptr to the end of 'lst' (which is '\0')
|
------------------------------------------------------------------------------------------*/

PUBLIC U8 GENERIC * Str_GetNthWord( U8 GENERIC const *lst, U8 n )
{
   U8 rIDATA wordCnt, 
             ch;      // current char
   BIT       isSpc;   // is a space?

   for( isSpc = 1, wordCnt = 0;; lst++ )
   {
      ch = *lst;                             // next char
      
      if( ch == 0 )                          // is end of list?
         { return (U8 GENERIC *)lst; }       // then return end ptr
   
      if( isSpc )                            // last was space?
      {
         if( !Str_Delimiter(ch) )            // and this isn't?
         {
            isSpc = 0;                       // mark as NOT space

            if( wordCnt == n )               // nth word?
            {
               return (U8 GENERIC *)lst;     // then return ptr to its 1st char
            }
         }
      }
      else                                   // else last was char
      {
         if( Str_Delimiter(ch) )             // but now a space? (so we have next word)
         {
            isSpc = 1;                       // mark it
            wordCnt++;                       // and bump word count
         }
      }   
   }
   #if _TOOL_IS != TOOL_CC430                // Except for CC430, which complains that this statement is unreachable....
   return 0;                                 // should never return from here, but keeps compiler happy
   #endif
}

// -------------------------------- eof ------------------------------------- 
