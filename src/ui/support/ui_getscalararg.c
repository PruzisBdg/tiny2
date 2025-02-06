/*---------------------------------------------------------------------------
|
|                Tiny Lib - Input pins
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "wordlist.h"
#include "sysobj.h"
#include "hostcomms.h"

#include "tty_ui.h"
#include "tiny1util.h"
#include "arith.h"
#include "util.h"



extern C8 * skipUnaryPrefix(C8 * p);

/* To read arguments of the form 'MyVec.min', include this file directly in your
   project and define 'PARSE_COMPOSITE_VECS'. in RIDE 8051 projects at least, the local
   version of the function will override the version imported from tty_ui.lib.
*/
#define PARSE_COMPOSITE_VECS


#ifdef PARSE_COMPOSITE_VECS

#include "class.h"
#include "vec.h"              // S_Vec

/*-----------------------------------------------------------------------------------------
|
|  applyUnaryPrefix()
|
|  Apply '-' (minus) or '!' (not) to 'n', return result.
|
------------------------------------------------------------------------------------------*/

PRIVATE S16 applyUnaryPrefix(S16 n, U8 prefix)
{
   return
      prefix == '-'                    // Was prefixed with '-'?
         ? -n                          // then flip sign
         : (
            prefix == '!'              // else was prefixed with '!'?
               ? (n == 0 ? 1 : 0)      // then do NOT -> boolean
               : n);                   // else return value unmodified.
}


/*-----------------------------------------------------------------------------------------
|
|  UI_GetScalarArg()
|
|  Return the value of scalar specified at position 'idx' in argument list 'args'. The
|  scalar may be a literal number or a scalar object.
|
|  If 'scale' <> 0 and the value is literal then the literal is read as a float and
|  divided by 'scale'. If 'scale' == 0 then literal is read as an integer, either
|  decimal or hex.
|
|  Returns the value of the scalar. If the argument isn't a Scalar or an literal number
|  OR if there's no nth arg, then returns 0.
|
------------------------------------------------------------------------------------------*/


PUBLIC S16 UI_GetScalarArg(C8 *args, U8 idx, float scale )
{
   S_Obj CONST * obj;
   float    f;
   C8       *p;
   U16      n;

   p = Str_GetNthWord(args, idx);                                 // Get the arg

   if( Str_EndOfLineOrString(*p))                                 // no nth arg?
   {
      return 0;                                                   // so return 0
   }
   else                                                           // else there is an nth arg.
   {
      /* Try to get an object from the putative name. If the name was prefixed
         with a '-' (minus) or '!' (not) then skip past these to the start of
         the name proper.
      */
      if( NULL != (obj = GetObj((C8 GENERIC *)skipUnaryPrefix(p))) )        // Is an object of some kind?
      {
         return applyUnaryPrefix(UI_GetIntFromObject(obj, p), *p); // Get it's value, apply '-' or '!' if that prefix exists
      }
      else                                                        // else not an object... see if it's its a literal number
      {
         if( ReadDirtyBinaryWord(p, &n))                          // Read e.g 0x1000_1100?
         {
            return n;                                             // then return that, no scale applied
         }
         else                                                     // else try demincal or hex.
         {
            if( !ReadASCIIToFloat(p, &f) )                        // Try reading decimal or hex, failed?
            {
               return 0;                                          // then say failed
            }
            else                                                  // else got a number
            {           // return the number scaled, but if 'scale' == 0 then don't use 'scale'
               return ClipFloatToInt(f/(scale == 0.0 ? 1.0 : scale));
            }
         }
      }
   }
}


#else // PARSE_COMPOSITE_VECS

PUBLIC S16 UI_GetScalarArg(U8 *args, U8 idx, float scale )
{
   S_Obj CONST * obj;
   float    f;
   U8       *p;

   p = (U8*)Str_GetNthWord(args, idx);                            // Get the arg

   if( Str_EndOfLineOrString(*p))                                 // no nth arg?
   {
      return 0;                                                   // so return 0
   }
   else                                                           // else there is an nth arg.
   {
      /* Try to get an object from the putative name. If the name was prefixed
         with a '-' (minus) or '!' (not) then skip past these to the start of
         the name proper.
      */
      if( obj = GetObj((U8 GENERIC *)skipUnaryPrefix(p)) )        // Is an object of some kind?
      {
         return                                                   // then get its value, assuming its a scalar
            *p == '-'                                             // Was prefixed with '-'?
               ? -Obj_ReadScalar(obj)                             // then flip sign
               : (
                  *p == '!'                                       // else was prefixed with '!'?
                     ? (Obj_ReadScalar(obj) == 0 ? 1 : 0)         // then do NOT -> boolean
                     : Obj_ReadScalar(obj));                      // else return value unmodified.
      }
      else                                                        // else not an object... see if it's its a literal number
      {
         if( !ReadASCIIToFloat((U8 const *)p, &f) )               // Try reading a number, failed?
         {
            return 0;
         }
         else                                                     // else got a number
         {           // return the number scaled, but if 'scale' == 0 then don;t use 'scale'
            return ClipFloatToInt(f/(scale == 0.0 ? 1.0 : scale));
         }
      }
   }
}
#endif // PARSE_COMPOSITE_VECS

// ------------------------------------ eof -----------------------------------------------
