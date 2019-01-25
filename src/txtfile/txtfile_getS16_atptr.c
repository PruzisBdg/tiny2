/*---------------------------------------------------------------------------
|
|                    Tiny1 Text Stores Library
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "txtfile.h"

/*-----------------------------------------------------------------------------------------
|
|  File_GetS16AtPtr()
|
|  Read an integer from a text file at address 'p'.
|
|  This function handles switching to and from the code bank where the text
|  files are.
|  
------------------------------------------------------------------------------------------*/

PUBLIC S16 File_GetS16AtPtr(S16 CONST *p)
{
   S16 n;
   
   File_EnterTextBank();      // Must goto text file bank to read
   n = *p;                    // Read the integer
   File_LeaveTextBank();      // Restore original bank settings.
   return n;
}

// -------------------------------- eof ------------------------------------------------


   
