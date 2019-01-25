/*---------------------------------------------------------------------------
|
|                 Tiny1 support 
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "sysobj.h"
#include "class.h"
#include "tiny1util.h"


/*-----------------------------------------------------------------------------------------
|
|  Obj_ReadScalar
|
|  Returns the value of a readable scalar object 'obj'. If 'obj' isn't one of these
|  returns 0 (but you should have checked beforehand)
|
------------------------------------------------------------------------------------------*/

PUBLIC S16 Obj_ReadScalar( S_Obj CONST * obj ) 
{ 
   if( Obj_IsAScalar(obj) )                                          // can read this?
   {
      return (*(GetClass(obj->type)->readFunc))( obj->addr );   // then call the classes' read function
   }
   else
   {
      return 0;                  // else return a polite number
   }
}

// ----------------------------- eof ----------------------------------------



 
