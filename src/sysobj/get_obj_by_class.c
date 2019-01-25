/*---------------------------------------------------------------------------
|
|                          Tiny1 - System Object Handling
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "sysobj.h"

/*-----------------------------------------------------------------------------------------
|
|  FUNCTION: GetObjByClass()
|
|  Return the entry in the object list for object 'name', provided the object is a 'class.
|  Return 0 if 'name' isn't in the list or if the object isn't of 'class'.
|  list.
|
------------------------------------------------------------------------------------------*/

PUBLIC S_Obj CONST * GetObjByClass( U8 GENERIC const *name, U8 theClass )
{
   S_Obj   CONST * rIDATA obj;

   if( !(obj = GetObj(name)) )
   { 
      return 0;
   }
   else if( obj->type != theClass )
   {
      return 0;
   }
   else
   {
      return obj;
   }
}

// --------------------------------- eof ------------------------------------ 
