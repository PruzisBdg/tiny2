/*---------------------------------------------------------------------------
|
|                Tiny Lib - Class Queries
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "sysobj.h"
#include "class.h"

/*---------------------------------------------------------------------------
|
|  Return the flags for the class of object 'obj'. Theese are 'read-only'
|  scalar / vector etc.
|
|--------------------------------------------------------------------------*/

PUBLIC U8 GetClassFlags( S_Obj CONST * obj )
{
   if( !obj )
      { return 0; }
   else
      { return GetClass(obj->type)->flags; }
}


 
