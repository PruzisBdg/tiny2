/*---------------------------------------------------------------------------
|
|                       Compares Scalars Only
|
|  UI_Compare_Vec() is the vector-friendly version of this routines. See its
|  comments in compare_vec.c.
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "wordlist.h"
#include "sysobj.h"
#include "class.h"

#include "tty_ui.h"
#include "romstring.h"


//  Routines common to UI_Compare_Vec() and UI_Compare_NoVec() are in compare.c

extern CONST C8 listOfCompareTypes[];
extern float GetArgScale(C8 *args);
extern U8 compare_Ints( S16 *nums, U8 compareType );
extern void compare_PrintLimitError( S_Obj CONST * obj, S16 *nums, U8 errNum );


PUBLIC U8 CONST Compare_NoVec_Help[] =
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
   WrStrLiteral("1st arg must be Scalar\r\n");
}

PUBLIC U8 UI_Compare_NoVec(C8 *args)
{
   U8    compareType,
         errNum = 0;

   S16   nums[3];
   float scale;

   S_Obj CONST * obj;


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

      if( 0 != (errNum = compare_Ints(nums, compareType)))  // compare 1st arg against limit(s), error?
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
   else
   {
      tellBad1stArg();
   }
   return 0;
}

// ------------------------------------ eof -----------------------------------------------
