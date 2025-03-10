/*---------------------------------------------------------------------------
|
|                Tiny Lib - Class Queries
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "sysobj.h"
#include "class.h"

/*-----------------------------------------------------------------------------------------
|
|  GetClassName()
|
|  Returns the name of a class 'classID'; 0 if the 'classID' isn't legal.
|
------------------------------------------------------------------------------------------*/

PUBLIC C8 CONST * GetClassName(U8 classID)
{
   if( !GetClass(classID) )
   {
      return 0;
   }
   else
   {
      return GetClass(classID)->name;
   }
}

// ----------------------------- eof ------------------------------------------- 
