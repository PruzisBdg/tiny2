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
|  File_NumberArrayStart()
|
|  Return a pointer to the 1st number in Flash file number store 'fileNum'. Returns 0 if 
|  there are no numbers OR if the file is not a numeric store OR if 'fileNum' doesn't 
|  index an file.
|
------------------------------------------------------------------------------------------*/

PUBLIC S16 CONST * File_NumberArrayStart(U8 fileNum)
{
   return File_NumberStoreCount(fileNum)              // Is a number store?
            ? &((S16 CONST *)File_Read(fileNum))[2]   // then return pointer to 1st number, which is after the count.
            : 0;                                      // else return zero
}

// -------------------------------- eof ------------------------------------------------


   
