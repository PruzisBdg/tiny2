/*---------------------------------------------------------------------------
|
|                 Tiny1 support 
|
|--------------------------------------------------------------------------*/

#ifndef TINY1UTIL_H
#define TINY1UTIL_H

#include "common.h"
#include "sysobj.h"

PUBLIC S16 Obj_ReadScalar( S_Obj CONST * obj );
PUBLIC void Obj_WriteScalar( S_Obj CONST * obj, S16 n );

#define _PrintScalarNano_AddName  _BitM(0)
#define _PrintScalarNano_AddUnits _BitM(1)
#define _PrintScalarNano_Newline  _BitM(2)
#define _PrintScalarNano_Full  (_PrintScalarNano_AddName | _PrintScalarNano_AddUnits)
#define _PrintScalarNano_NumberOnly  0
PUBLIC BIT Obj_PrintScalar_Nano( U8 hdl, U8 format );
PUBLIC BIT Obj_PrintNumberAsScalar_Nano( S16 n, U8 hdl, U8 format );


#endif // TINY1UTIL_H
 
