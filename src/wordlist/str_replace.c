/*---------------------------------------------------------------------------
|
|                          Word lists
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "wordlist.h"

/*-----------------------------------------------------------------------------
|
|  Str_Replace()
|
|  Replace 'numToReplace' words in 'str' with 'numToInsert' words from 'toFind'.
|
|  Replacement is at the 1st SINGLE word in 'str' which matches the 1st WORD in
|  'to Find'.
|
|  If 'numToReplace' exceeds the number of words in the remainder of the string
|  then the whole remainder is replaced.
|
|  If 'numToInsert' exceeds the number of words in 'toFind' then all the words
|  in 'toFind' are inserted.
|
|  If there is no match then 'str' is unchanged.
|
------------------------------------------------------------------------------*/

void Str_Replace( U8 GENERIC *str, 
                  U8 GENERIC const *toFind, 
                  U8 GENERIC const * replaceWith, 
                  U8 numToReplace, 
                  U8 numToInsert )
{
   U8 findIdx;

   if( (findIdx = Str_FindWord(str, toFind)) != _Str_NoMatch)
   {
      Str_Delete(str, findIdx, numToReplace);
      Str_Insert(str, replaceWith, findIdx, numToInsert);
   }
}

// ----------------------------- eof -------------------------------------- 
