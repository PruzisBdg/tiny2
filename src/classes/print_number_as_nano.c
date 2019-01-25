/*---------------------------------------------------------------------------
|
|              Nano - printout
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "sysobj.h"
#include "class.h"
#include "tiny1util.h"

/*-----------------------------------------------------------------------------------------
|
|  Obj_PrintNumberAsScalar_Nano()
|
|  Print the value of a 'n' in the format for scalar object hdl:
|
|     "<Object name> = <value> <units>"
|
|  The object must has an S_ObjIO spec which tells how to name, scale and print the
|  value. The object must also be a readable scalar.
|
|  This function is akin to Obj_PrintScalar_Nano(). It is used to print the filtered
|  or averaged value of a scalar in the same format as the scalar itself.
|
|  Return 1 if 'n' was printed, else 0.
|
------------------------------------------------------------------------------------------*/

PUBLIC BIT Obj_PrintNumberAsScalar_Nano( S16 n, U8 hdl, U8 format )
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
            printf("object %d = %d\r\n", hdl, n);
            return 1;
         }
         else
         {
            // Print with or without name and units.
            printf( BSET(format, _PrintScalarNano_AddName) ? " %s = " : " ", io->name);      // Either name or just a leading space
            printf(io->outFormat, io->outScale * (float)n);   // Print value
            printf( BSET(format, _PrintScalarNano_AddUnits) ? "%s " : " ", io->units);       // then either units or just a trailing space
            if( BSET(format, _PrintScalarNano_Newline)) { printf("\r\n"); }
            return 1;
         }
      }
   }
}


// ------------------------------ eof -----------------------------------
