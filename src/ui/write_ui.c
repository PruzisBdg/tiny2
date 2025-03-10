/*---------------------------------------------------------------------------
|
|                 TTY User Interface - 'write' command
|
|--------------------------------------------------------------------------*/\

#include "libs_support.h"
#include "common.h"
#include "tty_ui.h"
#include "hostcomms.h"
#include "sysobj.h"           // S_Obj
#include "class.h"
#include "wordlist.h"      // Str_FindWord()
#include "tiny1util.h"
#include "romstring.h"


/*-----------------------------------------------------------------------------------------
|
|  UI_WriteObj()
|
|  Command handler to write the value of a Scalar or Vector. The formats are:
|
|     write scalar n [raw]             - writes 'n' to 'scalar' (of that name)
|     write vector n0, n1... np [raw]  - writes n0 - np to 'vector' [raw]
|     write vector n [raw]             - fill whole 'vector' with 'n'
|
|  If 'raw' is appended to the arg list OR the variable has no scaling specified then the 
|  numbers are written literally to the variable's internal storage. Then they should be
|  integers (if float they will be rounded). Otherwise the numbers are scaled by the 
|  scaling factor for that variable before being written.
|
|
------------------------------------------------------------------------------------------*/

   #ifdef INCLUDE_HELP_TEXT
PUBLIC U8 CONST UI_Write_Help[] = 
"Writes the value of a Scalar or Vector\r\n\
Usage: <object_to_write> <n0_to_write [n1_to_write..]> [raw]\r\n\
\r\n\
Examples:\r\n\
   write scalar1 27.5\r\n\
   write scalar1 275 raw\r\n\
   write vector1 2.9 7.1 4.2 2.1 6.4   - write 'vector1', which has 5 elements\r\n\
   write vector3 290 710 420 raw       - write just 1st 3 elements of 'vector1'\r\n\
";
   #endif // INCLUDE_HELP_TEXT

PUBLIC U8 UI_WriteObj( C8 *args )
{
   U8    c, 
         numsInArgList,
         numsToWrite,
         argCnt;

   S_Vec *v;

   S_Obj   CONST * obj;
   S_ObjIO CONST * io;

   if( !(obj = UI_GetAnyObj_MsgIfFail(args)) )                 // 1st arg isn't an object?
   {
      return 0;
   }
   else
   {
      argCnt = Str_WordCnt( args ) - 1;                        // number args excepting the object name

      if( !UI_RawInArgList(args) )                             // arg list doesn't contain 'raw'?
      {
         io = GetObjIO(_StrConst(obj->name));                  // then cache the IO format, if one exists
         numsInArgList = argCnt;                               // assume the remaining args are number
      }
      else                                                     // else 'raw' is in arg list
      {
         io = 0;                                               // so won't be scaling input, force 'io' to zero for UI_IntFromArgs()
         numsInArgList = argCnt - 1;                           // and assume remaining args are numbers
      }
      
      if(obj->type == _Class_Vec)                              // vector?
      {
         v = (S_Vec*)obj->addr;
   
         if( numsInArgList == 1 )                              // just one number?
         {                                                     // then fill the whole Vector with it
            Vec_Fill( v, UI_IntFromArgs(args, io, 1) );
         }
         else                                                  // else multiple numbers
         {
            numsToWrite = MIN(numsInArgList, Vec_Size(v));     // Write no more numbers than the vector will hold
   
            for( c = 0; c < numsToWrite; c++ )                 // and fill vector elements from the argument list
            {
               v->nums[c] = UI_IntFromArgs(args, io, c+1);     // 1st number is arg 1, so 'c+1'
            }
         }
      }
      else if( Obj_IsWritableScalar(obj) )                     // scalar?
      {
         Obj_WriteScalar(obj, UI_IntFromArgs(args, io, 1));
      }
      else                                                     // else neither, so not writable
      {
         WrStrLiteral("Can't write this!");
      }
   }
   return 1;
}




 
