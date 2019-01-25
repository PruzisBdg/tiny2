/*---------------------------------------------------------------------------
|
|                Tiny Lib - Class Queries
|
|--------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
|
|  Obj_IsWritableVector()
|
|  Return 1 if 'obj' is a writable vector
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "sysobj.h"
#include "class.h"

PUBLIC U8 Obj_IsWritableVector( S_Obj CONST * obj)
{
   return ALL_SET( GetClassFlags(obj), _ClassFlags_IsAVector | _ClassFlags_IsWritable );
}

 
