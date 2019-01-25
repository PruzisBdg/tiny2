/*---------------------------------------------------------------------------
|
|                Tiny Lib - Class Queries
|
|--------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
|
|  Obj_IsAVector()
|
|  Return 1 if 'obj' is a vector
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "sysobj.h"
#include "class.h"

PUBLIC U8 Obj_IsAVector( S_Obj CONST * obj)
{
   return BSET( GetClassFlags(obj), _ClassFlags_IsAVector );
}

 
