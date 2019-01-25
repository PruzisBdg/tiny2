/*---------------------------------------------------------------------------
|
|                    Space-delimited Word lists
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "wordlist.h"

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

// ---------------------------- eof ---------------------------------------- 
