/*---------------------------------------------------------------------------
|
|                    Space-delimited Word lists
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "wordlist.h"

/*-----------------------------------------------------------------------------------------
|
|  Str_CopyNthWord()
|
|  Fetch the nth word in lst into 'out', null-terminated. 
|
|  Returns the number of chars inserted into 'out', 0 if there aren't enough words in 
|  'lst' OR if the word is longer than 'maxChars', i.e it won't fit into 'out'.
|
|  If there's no nth word then 'out' is '\0'. If word is longer than 'maxChars' then
|  'out' contains the 1st 'maxChars' of the word. 
|
|  Note that 'maxChars' does NOT include the terminating '\0' so that the output buffer
|  should be at least 'maxChars' + 1.
|
------------------------------------------------------------------------------------------*/

PUBLIC U8 Str_CopyNthWord( U8 GENERIC const *lst, U8 *out, U8 n, U8 maxChars )
{
   U8 GENERIC * rIDATA p;
   U8 rIDATA c, ch;

   if( !(p = Str_GetNthWord( lst, n )) )  // no nth word in list?
   {
      out[0] = '\0';                      // 'out' is empty string
      return 0;                           // so return 0;
   }
   else                                   // else found it, copy to 'out'
   {
      for( c = 0;; c++ )
      {
         if( c >= maxChars )              // word won't fit in 'out'?
         {
            out[c] = '\0';                // 'out' is as many letters as will fit
            return 0;                     // so return 0;
         }
         ch = *(p + c);
         if( ch == 0 || ch == ' ')        // space or end of 'lst'
         {
            out[c] = '\0';                // terminate to string
            return c;                     // return chars copied
         }
         else
         {
            out[c] = ch;                  // else copy char.. and continue
         }
      }
   }
}

// ------------------------------ eof ---------------------------------------- 
