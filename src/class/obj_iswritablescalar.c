/*---------------------------------------------------------------------------
|
|                Tiny Lib - Class Queries
|
|--------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
|
|  Obj_IsWritableScalar()
|
|  Return 1 if 'obj' is a writable scalar
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "sysobj.h"
#include "class.h"

PUBLIC U8 Obj_IsWritableScalar( S_Obj CONST * obj)
{
   return ALL_SET( GetClassFlags(obj), _ClassFlags_IsAScalar | _ClassFlags_IsWritable );
}

 
