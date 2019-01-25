/*---------------------------------------------------------------------------
|
|                Tiny Lib - Class Queries
|
|--------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
|
|  Obj_IsWritable()
|
|  Return 1 if 'obj' is writable
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "sysobj.h"
#include "class.h"

PUBLIC U8 Obj_IsWritable( S_Obj CONST * obj)
{
   return BSET( GetClassFlags(obj), _ClassFlags_IsWritable );
}

 
