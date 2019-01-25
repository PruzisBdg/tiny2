/*---------------------------------------------------------------------------
|
|                 Tiny1 support 
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "sysobj.h"
#include "class.h"


/*-----------------------------------------------------------------------------------------
|
|  Obj_WriteScalar
|
|  Writes 'n' to writable scalar object 'obj'. If 'obj' isn't a legal type then no action
|
------------------------------------------------------------------------------------------*/

PUBLIC void Obj_WriteScalar( S_Obj CONST * obj, S16 n )
{
   if( Obj_IsWritableScalar(obj) )                             // can write this?
   {
      (*(GetClass(obj->type)->wrFunc))( obj->addr, n );   // then call the classes' write function
   }
}


// ----------------------------- eof ----------------------------------------
 
