/*---------------------------------------------------------------------------
|
|                    Space-delimited Word lists
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "wordlist.h"

/*-----------------------------------------------------------------------------------------
|
|  Str_EndOfLineOrString
|
|  This call is for those functions which use Str_GetNthWord() or Str_GetEndWord() but
|  apply these functions to lines terminated by CRLF instead of strings (which end with
|  '\0').
|
------------------------------------------------------------------------------------------*/

PUBLIC BIT Str_EndOfLineOrString(U8 ch)
{
   return ch == '\0' || ch == '\r' || ch == '\n';
}

// ---------------------------------- eof ------------------------------------------


 
