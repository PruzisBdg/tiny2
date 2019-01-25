/*---------------------------------------------------------------------------
|
|                    Space-delimited Word lists
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "wordlist.h"

/*-----------------------------------------------------------------------------------------
|
|  Str_WordsInStr()
|
|  Finds the 1st word in 'wordsToMatch' which matches ANY word in 'str'. Returns an index 
|  to the word in 'wordsToMatch' (NOT 'str') e.g.
|
|     Str_WordsInStr("one two three four",  "just four three")
|
|     returns 1 because 'four' in "just four three" is the 1st match.
|
|  In a typical use 'str' is to be checked against as set of legal choices in 'wordsToMatch'.
|  This function tells whether 'str' contains any of the choices and which one it was.
|
|  If no match returns '_Str_NoMatch' (0xFF).
|
------------------------------------------------------------------------------------------*/

PUBLIC U8 Str_WordsInStr( U8 GENERIC const *str, U8 GENERIC const *wordsToMatch )
{
   U8 c, cnt;

   cnt = Str_WordCnt(wordsToMatch);

   for( c = 0; c < cnt; c++ )                                  // for each word in 'wordsToMatch'
   {
      if( Str_WordInStr(str, Str_GetNthWord(wordsToMatch, c))) // that word is in 'str'?
      {
         return c;                                             // then return index to it.
      }
   }
   return _Str_NoMatch;                                        // else no matches between 'wordsToMatch' and 'str'
}

// ---------------------------------- eof ----------------------------------------- 
