/*---------------------------------------------------------------------------
|
|                 TTY User Interface - UI_PrintVector()
|
|--------------------------------------------------------------------------*/\

#include "libs_support.h"
#include "common.h"
#include "tty_ui.h"
#include "hostcomms.h"     // Comms_WrStrConst
#include "vec.h"              // S_Vec

#include <string.h>
#include "sysobj.h"           // S_Obj
#include "romstring.h"


/*-----------------------------------------------------------------------------------------
|
|  UI_PrintVector()
|
|  Print 'vec' with format and scale given in 'io'. If 'io' == 0 then print it as a list
|  of integers.
|
|	DATE        AUTHOR            DESCRIPTION OF CHANGE 
|	--------		-------------		-------------------------------------
|
------------------------------------------------------------------------------------------*/

PUBLIC void UI_PrintVector( S_Vec *vec, S_ObjIO CONST * io, BIT appendUnits)
{
   if( !io )                                                   // requested 'raw' OR there's no IO format for this vector?
   {                                                           // then print the raw numbers
      UI_PrintVec( vec, _RomStr("%i "), 0 );
   }
   else
   {                                                           
      UI_PrintVec( vec, io->outFormat, io->outScale );         // else print using the IO format specified
      if( appendUnits )                                        // Print with units?
         { Comms_WrStrConst(_StrConst(io->units)); }           // then append units string
   }
}

// ------------------------------------ eof ------------------------------------------------ 
