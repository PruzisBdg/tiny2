/*---------------------------------------------------------------------------
|
|                    Space-delimited Word lists
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "wordlist.h"

/*-----------------------------------------------------------------------------------------
|
|  Str_GetEndWord()
|
|  Return pointer to the last char of the 'n'th word in the delimited word list 'lst'. 
|  If there aren't enough words, returns pointer to the end of the last word. If the 
|  list is empty (no words), returns the end of the list ('\0')
|
------------------------------------------------------------------------------------------*/

PUBLIC U8 GENERIC * Str_GetEndWord( U8 GENERIC const *lst, U8 n )
{
   U8       wordCnt,       // counts through words
            ch,            // current char
            i,             // index through source string
            endLastWord;   
            
   BIT      isSpc;         // most recent char was a delimiter?

   for( i = 0, isSpc = 1, wordCnt = 0;; i++ )
   {
      ch = lst[i];                  // next char
      
      if( ch == 0 )                 // is end of list?
      { 
         if(isSpc)                  // preceded by a space?
         {
            if(wordCnt)             // but there was at least one word?
            {
               i = endLastWord;     // then return the end of it
               break;
            }
            else                    // else there were no words
            {
               break;               // so return the empty list '\0'
            }
         }
         else                       // else end-of-list was preceded by a char
         {
            i--;                    // then return it, even though we didn't get enough words
            break;                  
         }
      }
   
      if( isSpc )                   // last was space?
      {
         if( !Str_Delimiter(ch) )   // and this isn't?
         {
            isSpc = 0;              // mark as NOT space
         }
      }
      else                          // else last was char
      {
         if( Str_Delimiter(ch) )    // but now a space? (so we have next word)
         {
            endLastWord = i-1;      // Mark the end of this most recent word
                                    // in case there are too few words in the list (above)
            if( wordCnt == n )      // reached requested word?
            {
               i--;                 // return previous char, the last one of the word
               break;
            }
            else                    // else keep looking
            {
               isSpc = 1;           // mark it
               wordCnt++;           // and bump word count
            }
         }
      }   
   }
   return (U8 GENERIC *)(lst+i);
}

// ----------------------------------- eof -----------------------------------------------
