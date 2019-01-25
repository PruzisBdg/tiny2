/*---------------------------------------------------------------------------
|
|              Nano - printout
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "common.h"
#include "sysobj.h"
#include "class.h"
#include "tiny1util.h"

// This function must link to a printf() with floating point support.
extern int printf(C8 CONST *fmt, ...);

/*-----------------------------------------------------------------------------------------
|
|  Obj_PrintScalar_Nano()
|
|  Print the value of a scalar object as:
|
|     "<Object name> = <value> <units>"
|
|  The obejct must has an S_ObjIO spec which tells how to name, scale and print the
|  value. The object must also be a readable scalar.
|
|  Return 1 if the object was printed, else 0.
|
------------------------------------------------------------------------------------------*/

PUBLIC BIT Obj_PrintScalar_Nano( U8 hdl, U8 format )
{
   S_ObjIO CONST *io;

   if( !ObjExists_Nano(hdl) )
   {
      printf("object %d doesn't exist\r\n", hdl);
      return 0;
   }
   else
   {
      if( !Obj_IsAScalar(GetObj_Nano(hdl)) )
      {
         printf("object %d is not a scalar\r\n", hdl);
         return 0;
      }
      else
      {
         if( !(io = GetObjIO_Nano(hdl)))
         {
            printf("object %d = %d\r\n", hdl, Obj_ReadScalar(GetObj_Nano(hdl)));
            return 1;
         }
         else
         {
            // Print with or without name and units.
            printf( BSET(format, _PrintScalarNano_AddName) ? " %s = " : " ", io->name);      // Either name or just a leading space
            printf(io->outFormat, io->outScale * (float)Obj_ReadScalar(GetObj_Nano(hdl)));   // Print value
            printf( BSET(format, _PrintScalarNano_AddUnits) ? "%s " : " ", io->units);       // then either units or just a trailing space
            if( BSET(format, _PrintScalarNano_Newline)) { printf("\r\n"); }
            return 1;
         }
      }
   }
}


// ------------------------------ eof -----------------------------------
