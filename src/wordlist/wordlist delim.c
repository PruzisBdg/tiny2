/*---------------------------------------------------------------------------
|
|                    Space-delimited Word lists
|
|  Functions to search space-delimited word lists. Used to make command-line user
|  interfaces.
|
|     Str_FindWord()     - find word match in a list
|     Str_GetNthWord()   - pointer to nth word
|     Str_CopyNthWord()  - copy out the nth word
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "wordlist.h"

// If running short of IDATA elsewhere then put local vars into XDATA by undoing IDATA
// #define IDATA

// Definitions for delimiters and end of list

PRIVATE BIT delimiter(U8 ch)
{
   return ch == ' ';
}

PRIVATE BIT endOfList(U8 ch)
{
   return ch == '\0' || ch == '\r' || ch == '\n';
}

/*-----------------------------------------------------------------------------------------
|
|  Str_WordCnt
|
|  Returns the number of words in 'str'
|
------------------------------------------------------------------------------------------*/

PUBLIC U8 Str_WordCnt( U8 GENERIC const *str )
{
   U8 IDATA cnt, ch;
   BIT spc;

   spc = 1;                               // Assume (leading) spaces until get char
   for( cnt = 0;; str++ )                 // for each char in str
   {
      ch = *str;

      if( endOfList(ch) ) { return cnt; } // end of 'str'? return cnt.

      if( spc )                           // currently parsing spaces?
      {
         if( !delimiter(ch) )             // now got a char?
         {
         spc = 0;
         cnt++;                           // then its another word
         }
      }
      else                                // else currently parsing chars
      {
         if( delimiter(ch) ) { spc = 1; } // got space? flip to parsing spaces
      }
   }
}




/*-----------------------------------------------------------------------------------------
|
|  Str_FindWord()
|
|  Return index to the word in 'lst' which matches the FIRST OR ONLY word in 'str'. If no
|  match return '_Str_NoMatch'. The 1st word index is 0.
|
|  A word is one or more non-space characters.
|
------------------------------------------------------------------------------------------*/

PUBLIC U8 Str_FindWord( U8 GENERIC const *lst, U8 GENERIC const *str )
{
   U8 IDATA ch,            // current char from 'lst'
            idx = 0,       // indexes 'str'
            wordCnt = 0;   // counts through words in 'lst'

   BIT      wasSpc = 1;    // if last char was a space
   BIT      neq = 0;       // when comparing words, '1' if a mismatch

   while(1)
   {
      ch = *lst++;                              // next char from list

      if( endOfList(ch) )                       // end of list?
      {
         if( neq == 0 &&                        // match so far AND
           (str[idx] == 0 || str[idx] == ' '))  // end of 'str' OR end of 1st word in 'str'
         {
            return wordCnt;                     // then found match for 'str'; return index to matching word
         }
         else
         {
            return _Str_NoMatch;                // else no match in 'lst'
         }
      }
      else if( delimiter(ch) )                  // next char is space?
      {
         if( wasSpc == 0 )                      // last char wasn't space (AND have space now)?
         {
            wasSpc = 1;                         // mark it now

            if( neq == 0 &&                     // match so far AND
              (str[idx] == 0 || str[idx] == ' '))  // end of 'str' OR end of 1st word in 'str'
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


/*-----------------------------------------------------------------------------------------
|
|  Str_WordInStr()
|
|  Return 1 if 'theWord' is anywhere in 'str', else 0
|
|
------------------------------------------------------------------------------------------*/

PUBLIC BIT Str_WordInStr( U8 GENERIC const *str, U8 GENERIC const *theWord )
{
   return Str_FindWord(str, theWord) != _Str_NoMatch;
}



/*-----------------------------------------------------------------------------------------
|
|  Str_WordsInStr()
|
|  Finds the 1st word in 'wordsToMatch' which has a match in 'str'. Returns an index to the
|  word (in 'wordsToMatch') e.g.
|
|     Str_WordsInStr("one two three four",  "just four three")
|
|     returns 1 because 'four' in "just four you" is the 1st match.
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



/*-----------------------------------------------------------------------------------------
|
|  Str_GetNthWord()
|
|  Return pointer to the 'n'th word in the space-delimited word list 'lst'. If there
|  aren't enough words, returns ptr to the end of 'lst' (which is '\0')
|
------------------------------------------------------------------------------------------*/

PUBLIC U8 * Str_GetNthWord( U8 GENERIC const *lst, U8 n )
{
   U8 IDATA wordCnt,
            ch;      // current char
   BIT      isSpc;   // is a space?

   for( isSpc = 1, wordCnt = 0;; lst++ )
   {
      ch = *lst;                 // next char

      if( endOfList(ch) )        // is end of list?
         { return lst; }         // then return end ptr

      if( isSpc )                // last was space?
      {
         if( !Str_Delimiter(ch) )     // and this isn't?
         {
            isSpc = 0;           // mark as NOT space

            if( wordCnt == n )   // nth word?
            {
               return lst;       // then return ptr to its 1st char
            }
         }
      }
      else                       // else last was char
      {
         if( delimiter(ch) )     // but now a space? (so we have next word)
         {
            isSpc = 1;           // mark it
            wordCnt++;           // and bump word count
         }
      }
   }
   return 0;                     // should never return from here, but keeps compiler happy
}


/*-----------------------------------------------------------------------------------------
|
|  Str_CopyNthWord()
|
|  Fetch the nth word in lst into 'out', null-terminated.
|
|  Returns the number of chars inserted into 'out', 0 if there aren't enough words in
|  'lst' OR if the word is longer than 'maxChars', i.e it won't fit into 'out'.
|
|  Note that 'maxChars' does NOT include the terminating '\0' so that the output buffer
|  should be at least 'maxChars' + 1.
|
------------------------------------------------------------------------------------------*/

PUBLIC U8 Str_CopyNthWord( U8 GENERIC const *lst, U8 *out, U8 n, U8 maxChars )
{
   U8 GENERIC const * IDATA p;
   U8 IDATA c, ch;

   if( !(p = Str_GetNthWord( lst, n )) )     // no nth word in list?
   {
      out[0] = '\0';                         // 'out' is empty string
      return 0;                              // so return 0;
   }
   else                                      // else found it, copy to 'out'
   {
      for( c = 0;; c++ )
      {
         if( c >= maxChars )                 // word won't fit in 'out'?
         {
            out[0] = '\0';                   // 'out' is empty string
            return 0;                        // so return 0;
         }
         ch = *(p + c);
         if( endOfList(ch) || delimiter(ch) ) // space or end of 'lst'
         {
            out[c] = '\0';                   // terminate to string
            return c;                        // return chars copied
         }
         else
         {
            out[c] = ch;                     // else copy char.. and continue
         }
      }
   }
}

// --------------------------------------- eof -----------------------------------------
