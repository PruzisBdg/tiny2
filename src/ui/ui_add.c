/*---------------------------------------------------------------------------
|
|                    Tiny1 UI - Adds Scalars
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "wordlist.h"
#include "sysobj.h"
#include "hostcomms.h"
#include "class.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "tty_ui.h"
#include "tiny1util.h"


PRIVATE void tellBad1stArg(void)
{
   Comms_WrStrConst("1st arg must be Scalar or Vector\r\n");
}


PUBLIC float UI_GetScaleForLiteralArgs(U8 *args)
{
   S_ObjIO CONST * io;

   if( ((io = GetObjIO(GetObj(args)->name)) != 0)  // Vector or Scalar to be compared has a scaling?
        && !UI_RawInArgList(args) )                // AND 'raw' not specified
      { return io->outScale; }                     // then return scale factor for literals based on obejct type
   else
      { return 1.0; }                              // else literal numbers will be used unscaled.
}

/*-----------------------------------------------------------------------------------------
|
|  UI_Add()
|
|  Command handler adds scalars, vectors and constants. The formats are:
|
|     add scalar1 literal [raw]        - scalar1 += literal
|
|     add scalar1 scalar2              - scalar1 += scalar2
|
|     add vec1 vec2                    - elementwise vector addition
|
|     add vec1 scalar2                 - scalar2 is added to each element of vec1
|
|     add vec1 literal                 - literal is added to each element of vec1
|
|  The 1st argument must be a writable scalar or vector type; the 2nd arg may be a
|  readable-only type or a literal.
|
|  RETURNS:  If arg list is well-formed and all comparisions are true (success) then
|            returns 1, else 0.
|
------------------------------------------------------------------------------------------*/

PUBLIC U8 CONST Add_Help[] =
"Usage:\r\n\
    add <item to add to> <value to add>\r\n\
\r\n\
   add Pump1Vdrv 1.4          - adds 1.4V to 'Pump1Vdrv', an actuator\r\n\
   add Pump1Vdrv 1400 raw     - adds 1.4V, expressed as raw value to 'Pump1Vdrv'\r\n\
";

PUBLIC U8 UI_Add(U8 *args)
{
   S_Obj CONST * dest;
   S_Obj CONST * src;

   S16 n;
   float f1, scale;

   if( !(dest = GetObj(args)) )
   {
      tellBad1stArg();
      return 0;
   }

   scale = UI_GetScaleForLiteralArgs(args);

   if( Obj_IsWritableScalar(dest) )
   {
      n = Obj_ReadScalar(dest);

      if( src = GetObj( Str_GetNthWord(args,1) ))
      {
         f1 = (float)Obj_ReadScalar(src);
      }
      else if( sscanf(Str_GetNthWord(args,1), "%f", &f1) != 1 )
      {
         f1 = 0;
      }
      f1 = (f1 / scale) + n;
      f1 = CLIP(f1, MIN_S16, MAX_S16);
      Obj_WriteScalar(dest, (S16)f1);
      return 1;
   }

   return 0;         // failed
}


// ------------------------------------- eof ----------------------------------------

