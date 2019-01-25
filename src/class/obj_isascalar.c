/*---------------------------------------------------------------------------
|
|                Tiny Lib - Class Queries
|
|--------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
|
|  Obj_IsAScalar()
|
|  Return 1 if 'obj' is scalar type
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "sysobj.h"
#include "class.h"

PUBLIC U8 Obj_IsAScalar( S_Obj CONST * obj)
{
   return BSET( GetClassFlags(obj), _ClassFlags_IsAScalar );
}

 
