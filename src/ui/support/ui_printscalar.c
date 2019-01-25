/*---------------------------------------------------------------------------
|
|                 TTY User Interface - UI_PrintScalar()
|
|--------------------------------------------------------------------------*/\

#include "libs_support.h"
#include "common.h"
#include "tty_ui.h"
#include "hostcomms.h"

#include <string.h>
#include "wordlist.h"      // Str_FindWord()
#include "romstring.h"


/*-----------------------------------------------------------------------------------------
|
|  UI_PrintScalar()
|
|  Print 'n' with format and scale given in 'io'. If 'io' == 0 then print just the (integer) 
|  value.
|
------------------------------------------------------------------------------------------*/

PUBLIC void UI_PrintScalar( S16 n, S_ObjIO CONST * io, U8 appendUnits )
{
   if( !io )                                                               // no IO format supplied?
   {                                               
      sprintf( PrintBuf.buf,  _StrLit("%i "), n  );                        // then print the raw number
   }
   else
   {                                                                       // else print using the specified IO format and scale
      sprintf( PrintBuf.buf,  _StrConst(io->outFormat), (float)(io->outScale * (float)n) );
      if( appendUnits )
         { strcat( PrintBuf.buf, _StrConst(io->units) ); }            // append units string if requested
   }
   PrintBuffer();
}

// -------------------------------------- eof --------------------------------------------- 
