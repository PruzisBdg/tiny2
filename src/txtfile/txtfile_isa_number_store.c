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
|  File_IsANumberStore()
|
|  Return 1 if text file 'fileNum' is a a legal text file AND that file is 
|  a binary number store.
|
|  A number store is tagged with '_File_NumericStoreTag'.
|
------------------------------------------------------------------------------------------*/

PUBLIC BIT File_IsANumberStore(U8 fileNum)
{
   return File_GetS16AtIdx( (S16 CONST *)File_Read(fileNum), 0) == _File_NumericStoreTag;   
}

// -------------------------------- eof ------------------------------------------------


 
