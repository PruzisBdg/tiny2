/*---------------------------------------------------------------------------
|
|                    Space-delimited Word lists
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "wordlist.h"

extern BIT wordChar(U8 ch);   // true if not space or '\0'

/*-----------------------------------------------------------------------------------------
|
|  Str_FindWord_CC()
|
|  Return index to the word in 'lst' which matches the FIRST OR ONLY word in 'str'. If no 
|  match return '_Str_NoMatch'. The 1st word index is 0.
|
|  A word is one or more non-space characters.
|
|  This is the version of Str_FindWord() which takes ROM pointers for both 'lst' and 'str'
|
------------------------------------------------------------------------------------------*/

PUBLIC U8 Str_FindWord_CC( U8 CONST *lst, U8 CONST *str )
{
   U8 rIDATA ch,           // current char from 'lst'
            idx = 0,       // indexes 'str'
            wordCnt = 0;   // counts through words in 'lst'

   BIT      wasSpc = 1;    // if last char was a space
   BIT      neq = 0;       // when comparing words, '1' if a mismatch

   while(1) 
   {
      ch = *lst++;                              // next char from list

      if( ch == '\0' )                          // end of list?
      {
         if( neq == 0 && !wordChar(str[idx]))   // match so far AND end of 'str' OR end of 1st word in 'str'
         {
            return wordCnt;                     // then found match for 'str'; return index to matching word
         }
         else
         {
            return _Str_NoMatch;                // else no match in 'lst'
         }
      }
      else if( ch == ' ' )                      // next char is space?
      {
         if( wasSpc == 0 )                      // last char wasn't space (AND have space now)?
         {
            wasSpc = 1;                         // mark it now

            if( neq == 0 && !wordChar(str[idx]))// match so far AND end of 'str' OR end of 1st word in 'str'
            {
               return wordCnt;                  // then found match for 'str'; return index to matching word
            }
            else                                // else words don't match, will keep looking
            {
               wordCnt++;                       // on to the next word
               neq = 0;                         // assume a match until found otherwise
               idx = 0;                         // reset 'str' ptr
            }
         }
      }
      else                                      // else a char other than space
      {
         wasSpc = 0;

         if( neq == 0 )                         // word matches so far?
         {
            if( ch != str[idx] )                // but this char doesn't match?
            {
               neq = 1;                         // then note the mismatch
            }        
            else                                // else will keep comparing
            {
               idx++;                           // so must bump 'str' ptr
            }
         }
      }
   }
}

// ------------------------------ eof --------------------------------------- 
