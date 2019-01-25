/*---------------------------------------------------------------------------
|
|                    Space-delimited Word lists
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "wordlist.h"
#include <string.h>

/*-----------------------------------------------------------------------------
|
|  Str_Delete()
|
|  Delete 'cnt' words from 'lst' from 'start' onwards.
|
------------------------------------------------------------------------------*/

PUBLIC void Str_Delete( U8 GENERIC *lst, U8 start, U8 cnt )
{
   #if _TOOL_IS == TOOL_CC430
   strcpy( (C8*)Str_GetNthWord(lst, start), (C8 const *)Str_GetNthWord(lst, start+cnt) );
   #else
   strcpy( Str_GetNthWord(lst, start), Str_GetNthWord(lst, start+cnt) );
   #endif
}

// --------------------------- eof ------------------------------- 
