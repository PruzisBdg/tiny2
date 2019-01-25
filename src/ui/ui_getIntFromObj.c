/*---------------------------------------------------------------------------
|
|                 TTY User Interface
|
|--------------------------------------------------------------------------*/\

#include "libs_support.h"
#include "common.h"
#include "tty_ui.h"        // UI_GetIntFromObject
#include "wordlist.h"      // Str_1stWordHasChar(), Str_FindWord()
#include "tiny1util.h"     // Obj_ReadScalar
#include "util.h"          // ReadASCIIToFloat()
#include "class.h"         // '_Class_Vec'
#include "vec.h"           // S_Vec



/*-----------------------------------------------------------------------------------------
|
|  UI_GetIntFromObject()
|
|  Return the value of 'obj', which must be a Scalar OR a scalar extracted from a Vector
|  e.g MyVec.min.
|
------------------------------------------------------------------------------------------*/

PRIVATE U8 CONST vectorOps[] = "min max mean avg";
typedef enum { E_Min, E_Max, E_Mean, E_Avg } E_VecOps;

PUBLIC S16 UI_GetIntFromObject(S_Obj CONST *obj, U8 *p)
{
   float    f;
   U8       idx;
   S_Vec    *v;
   
   if( obj->type == _Class_Vec )                            // Is a vector?
   {
      if( (idx = Str_1stWordHasChar(p, '.')) != _Str_NoMatch )    // Has a qualifier e.g 'MyVec.min'?
      {
         v = (S_Vec *)obj->addr;                            // Get the vector
         p = p + idx + 1;                                   // We have the vector object. Advance 'p' to the qualifier
               
         if( ReadASCIIToFloat(p, &f) )                      // Qualifier is a number?
         {
            return v->nums[(U8)f];                          // then use it as an index i.e 'MyVec.3 == MyVec[3]'
         }                                                  // else qualifier is not a number...
         else if( (idx = Str_FindWord(vectorOps, p)) != _Str_NoMatch )  // Is qualifier a vector operation?
         {
            switch(idx)                                     // Qualifier is a vector op..
            {                                               // Select which one to do.
               case E_Min:
                  return Vec_Min(v);
                  break;
                                 
               case E_Max:
                  return Vec_Max(v);
                  break;
                                 
               case E_Mean:
               case E_Avg:
                  return Vec_Mean(v);
                  break;
                  
               default:
                  return 0;
            }
         }
      }
      return 0;                                             // If anything above failed return 0.
   }
   else                                                     // else was not a vector
   {
      return Obj_ReadScalar(obj);                           // return value of teh scalar
   }
}


// -------------------------------- eof ---------------------------------------- 
