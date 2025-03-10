/*---------------------------------------------------------------------------
|
|                 TTY User Interface - UI_PrintScalar()
|
|--------------------------------------------------------------------------*/\

#include "common.h"
#include "tty_ui.h"
#include "hostcomms.h"

#include <string.h>
#include "wordlist.h"      // Str_FindWord()
#include "romstring.h"


/*-----------------------------------------------------------------------------------------
|
|  UI_SPrintScalar()
|
|  Print 'n' to 'out' with format and scale given in 'io'. If 'io' == 0 then print just 
|  the (integer) value.
|
------------------------------------------------------------------------------------------*/

PUBLIC void UI_SPrintScalar( C8 *out, S16 n, S_ObjIO CONST * io, U8 appendUnits )
{
   if( !io )                                                      // no IO format supplied?
   { 
      sprintf( out, _StrLit("%i "), n  );                         // then print the raw number
   }
   else
   {                                                              // else print using the specified IO format and scale
      sprintf( out,  _StrConst(io->outFormat), io->outScale * (float)n );
      if( appendUnits )
         { strcat( out, _StrConst(io->units) ); }                 // append units string if requested
   }
}

// -------------------------------------- eof --------------------------------------------- 
