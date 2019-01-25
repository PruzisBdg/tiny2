/*---------------------------------------------------------------------------
|
|                    Tiny1 Text Stores Library
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "txtfile.h"

/*-----------------------------------------------------------------------------------------
|
|  File_GetS16AtIdx()
|
|  Read an integer as 'idx' from the text file which start at 'fileStart'.
|
|  This function handles switching to and from the code bank where the text
|  files are.
|  
------------------------------------------------------------------------------------------*/

PUBLIC S16 File_GetS16AtIdx(S16 CONST *fileStart, S16 idx)
{
   return File_GetS16AtPtr(&fileStart[idx]);
}

// -------------------------------- eof ------------------------------------------------


  
