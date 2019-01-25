/*---------------------------------------------------------------------------
|
|        The inter-word delimiter for space-delimited word lists
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "wordlist.h"

/*-----------------------------------------------------------------------------------------
|
|  Str_Delimiter
|
|  In word string, words are separated by spaces commas or newlines.
|
------------------------------------------------------------------------------------------*/

PUBLIC BIT Str_Delimiter(U8 ch)
{
   if( ch == ' ' || ch == ',' || ch == '\t' || ch == '\r' || ch == '\n' || (Str_DiceComposites && ch == '.') )
      { return 1; }
   else
      { return 0; }
}

// ------------------------------ eof ----------------------------------------- 
