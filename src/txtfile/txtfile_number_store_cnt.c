/*---------------------------------------------------------------------------
|
|                    Tiny1 Text Stores Library
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "txtfile.h"
#include "txtfile_hide.h"

/*-----------------------------------------------------------------------------------------
|
|  File_NumberStoreCount()
|
|  Return the number of integers in the flash file number store indexed by
|  'fileNum'. Returns 0 if there are no number OR if the file is not a numeric
|  store OR if 'fileNum' doesn't index an file.
|
------------------------------------------------------------------------------------------*/

PUBLIC S16 File_NumberStoreCount(U8 fileNum)
{
   return File_IsANumberStore(fileNum)                               // Is a number store?
            ? File_GetS16AtIdx((S16 CONST *)File_Read(fileNum),1)    // then return the number count, in the 2nd integer.
            : 0;                                                     // else return zero
}

// -------------------------------- eof ------------------------------------------------


  
