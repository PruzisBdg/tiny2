/*---------------------------------------------------------------------------
|
|                    Space-delimited Word lists
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "wordlist.h"

/*-----------------------------------------------------------------------------------------
|
|  wordChar()
|
|  Used by Str_ routines (wordlist). Returns 1 if char is not a word delimiter or
|  end of word list.
|
------------------------------------------------------------------------------------------*/

PUBLIC BIT wordChar(U8 ch)
{
   if( ch == '\0' || ch == ' ' ) return 0; else return 1;
}

/*-----------------------------------------------------------------------------------------
|
|  Str_1stWordsMatch()
|
|  Return 1 if 1st word in word lost 'w1' matches the 1st word in list 'w2'. A word is one 
|  or more non-space chars, terminated by any non-word char.
|
|  Bypasses leading whitespace in 'w1' and 'w2'.
|
------------------------------------------------------------------------------------------*/

PRIVATE U8 GENERIC const * stripSpc(U8 GENERIC const *w)
{
   while( *w == ' ' )
      { w++; }
   return w;
}

PUBLIC BIT Str_1stWordsMatch( U8 GENERIC const *w1, U8 GENERIC const *w2 )
{
   U8 i;
   
   w1 = stripSpc(w1);
   w2 = stripSpc(w2);

   for(i = 0;;i++)
   {
      if( !wordChar(w1[i]) && !wordChar(w2[i]) )   // ended with match?
      {
         if(i == 0)                                // but didn't have word yet?
            { return 0; }                          // then no match
         else                                      // else at least 1 char matched
            { return 1; }                          // so success!
      }
      else if( w1[i] != w2[i] )                    // else mismatch?
         { return 0; }
   }
}

// ------------------------------- eof ----------------------------------- 
