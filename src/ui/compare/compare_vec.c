/*---------------------------------------------------------------------------
|
|       Test Fixture UI - Compares Scalars and Vector
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "wordlist.h"
#include "sysobj.h"
#include "class.h"

#include <string.h>
#include "tty_ui.h"
#include "romstring.h"
#include "arith.h"


//  Routines common to UI_Compare_Vec() and UI_Compare_NoVec() are in compare.c

extern CONST U8 listOfCompareTypes[];
extern float GetArgScale(U8 *args);
extern U8 compare_Ints( S16 *nums, U8 compareType );
extern void compare_PrintLimitError( S_Obj CONST * obj, S16 *nums, U8 errNum );


/*-----------------------------------------------------------------------------------------
|
|  compareVecElement()
|
|  Report errors to user
|
------------------------------------------------------------------------------------------*/

PRIVATE U8 compareVecElement( S_Obj CONST * obj, S16 *nums, U8 compareType, U8 idx )
{
   U8 errNum;

   if( errNum = compare_Ints(nums, compareType))
   {
      sprintf(PrintBuf.buf, "%s[%d]: ", obj->name, idx);
      PrintBuffer();
      compare_PrintLimitError(obj, nums, errNum);
   }
   return errNum;
}

/*-----------------------------------------------------------------------------------------
|
|  UI_Compare()
|
|  Command handler compares Scalars or Vectors. The formats are:
|
|     compare n1 <greater | less>  n2 [raw]  - if 'greater' then message if n1 > n2
|                                              if 'less' then message if n1 < n2
|
|     compare n1 <inside> n2 n3 [raw]        - if 'inside' message if n1 inside n2, n3
|                                              if 'outside' message if n1 outside n2, n3
|
|  'n1' may be (the name of) a Scalar or Vector. 'n2', 'n3' may be Scalar, Vectors or
|  literal numbers. How 'n2' and 'n3' are read depends on the type of variable and type
|  of comparsion.
|
|  If 'n1' is a Scalar then either of 'n2', 'n3' may either of Scalars or literals. Both
|  arguments are required if the comparision is 'inside', otherwise the 2nd argument is
|  ignored. If 'raw' is appended to the command the literals are taken to be internal
|  numbers, otherwise literals are scaled before being compared to the internal, integer,
|  value of the Scalar. Examples:
|
|     compare scalar1 greater 33.2;   
|        returns 1 if scalar1 > 33.2. '33.2' is scaled to the internal units for 'scalar1';
|        e.g if scaling is x100 then 33.2 will be converted to 3320.
|
|     compare scalar1 inside 1.7 10
|
|     compare scalar1 inside 2300 scalar2 raw
|        returns 1 if scalar1 is between 2300 and scalar 2. 2300 is read as a raw integer
|
|  If 'n1' is a Vector then n2 may be a Vector OR 'n2', 'n3' may be literals. If 'n2' is
|  a Vector then how it is read depends on the type of comparision. If 'greater' or 'less'
|  then 'n2' compared, element by element against 'n1'. If 'inside' then 'n2' is read as
|  a series of number-pairs, each pair being a lower limit followed by an upper limit.
|  Examples
|
|     compare vec1 inside 45 187.2
|        Return 1 if every element of 'vec1' is >45 and <187.2 (scaled to internal units
|        of 'vec1')
|
|     compare vec1 inside vec2   - where vec2 = [224 470 321 569 445 781]
|        1st element of vec1 must be >224 and < 470 (raw)
|        2nd element of vec1 must be >321 and < 569 (raw)
|        etc.
|
|     compare vec1 less 100 raw
|        Return 1 if every element of vec1 is < 100, read as a raw number.
|
|  
|  UI_Compare() prints an error message for each comparision which fails. The message
|  show the value and the limits. For vectors the message names the element which failed.
|
|
|
|  RETURNS:  If arg list is well-formed and all comparisions are true (success) then 
|            returns 1, else 0.
|  
------------------------------------------------------------------------------------------*/

extern float GetArgScale(U8 *args);


PUBLIC U8 CONST Compare_Vec_Help[] = 
"Usage:\r\n\
    <scalar type to compare> <comparision = 'greater''>' | 'less''<' | 'inside' |'equal''==' | 'notEqual' '!=' '<>'>...\r\n\
    <limit | lo limit> [hi limit] ['raw']\r\n\
\r\n\
   compare SupplyV less 11.7      - in volts\r\n\
   compare Pres1 inside 47.2 63   - pressure sensor 'Pres1' in kPa\r\n\
   compare SupplyV <    11700 raw - in internal units\r\n\
";

PRIVATE void tellBad1stArg(void)
{
   WrStrLiteral("1st arg must be Scalar or Vector\r\n");
}

// *** Duplicated in compare.c
typedef enum 
{  compare_Inside, 
   compare_Greater, compare_GreaterSym, 
   compare_Less, compare_LessSym, 
   compare_Equal, compare_EqualSym, 
   compare_NotEqual, compare_NotEqualSym, compare_NotEqualCSym
} E_Compares;



PUBLIC U8 UI_Compare_Vec(U8 *args)
{
   U8    c, 
         numCompares, 
         numLimits,
         compareType,
         bad,
         errNum = 0;

   S16   nums[3];
   S_Vec *v1, *v2;
   float scale;

   S_Obj CONST * obj;
   S_Obj CONST * obj1;


   if( !(obj = GetObj(args)) ) 
   {
      tellBad1stArg();
      return 0;
   }

   if( (compareType = Str_WordsInStr(args, _StrConst(listOfCompareTypes))) == _Str_NoMatch)
   {
      WrStrLiteral("specify 'inside' 'greater' or 'less'\r\n");
      return 0;
   }

   scale = GetArgScale(args);                               // Get scale for any literal args

   if( Obj_IsAScalar(obj) )                                 // 1st arg is a Scalar?
   {
      nums[0] = UI_GetScalarArg(args, 0, scale);            // so get its value
      nums[1] = UI_GetScalarArg(args, 2, scale);            // and value of the next number (the 3rd arg)
      nums[2] = UI_GetScalarArg(args, 3, scale);            // and the 3rd number (4th arg), if it exists

      if( errNum = compare_Ints(nums, compareType))          // compare 1st arg against limit(s), error?
      {
         sprintf(PrintBuf.buf, "%s: ", obj->name);
         PrintBuffer();
         compare_PrintLimitError(obj, nums, errNum);
         return 0;
      }
      else
      {
         return 1;
      }
   }
   else if( obj->type == _Class_Vec )                       // else 1st arg is a Vector?
   {
      v1 = (S_Vec *)obj->addr;                              
      numCompares = Vec_Size(v1);                           // Will test each element in source vector
                                                            // (unless comparsion vector has fewer limits)

      if( (obj1 = GetObjByClass(Str_GetNthWord(args,2), _Class_Vec)) )  // 3rd arg (the limits) is a Vector
      {
         v2 = (S_Vec *)obj1->addr;
         numLimits = Vec_Size(v2);                          // elements in comparision vector

         if(compareType == compare_Inside)                  // need upper and lower limits
            { numLimits /= 2; }                             // then have half as many ets of limits as vector elements

         numCompares = MinU8(numCompares, numLimits);       // compare no more elements than in the smaller vector
               
         for( c = 0, bad = 0; c < numCompares; c++ )        // for each comparison to be done
         {
            nums[0] = v1->nums[c];                          // value to be checked against limits

            if(compareType == compare_Inside)               // check inside limits?
            {
               nums[1] = v2->nums[2*c];                     // then lower limit is 1st row
               nums[2] = v2->nums[(2*c) + 1];               // upper limit is 2nd row
            }
            else                                            // else check greater / less
            {
               nums[1] = v2->nums[c];                       // limit is in the 1st row
            }
            bad |= compareVecElement(obj, nums, compareType, c);   // compare against limit(s), report upper/lower
         }
         if( bad ) return 0; else return 1;
      }
      else                                                  // else assume 3rd arg is a number, the 1st of two
      {
         nums[1] = UI_GetScalarArg(args, 2, scale);         // get 3rd arg, the lower limit for all vector elements
         nums[2] = UI_GetScalarArg(args, 3, scale);         // get 4th arg, the upper limit, if it exists

         for( c = 0, bad = 0; c < numCompares; c++ )        // for each element in source vector (the 1st arg)
         {
            nums[0] = v1->nums[c];                          // get the value
            bad |= compareVecElement(obj, nums, compareType, c);   // compare against limit(s), report any outside limits
         }
         if( bad ) return 0; else return 1;
      }
   }
   else
   {
      tellBad1stArg();
   }
   return 0;
}

// ----------------------------------------- eof --------------------------------------- 
