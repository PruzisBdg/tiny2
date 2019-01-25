/*---------------------------------------------------------------------------
|
|            Test Fixture Support: Compares Scalars and Vectors
|
|  Routines common to 'vector' and 'no-vector' versions of UI_Compare()
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "wordlist.h"
#include "sysobj.h"
#include "hostcomms.h"

#include <string.h>
#include "romstring.h"

/*-----------------------------------------------------------------------------------------
|
|  compare_PrintNum()
|
|  Print 'n' with the formatting and units of 'obj'. If '_PrintNum_Flags_Raw is set
|  then print the number unformatted (and without units), even 'obj' specifies an IO
|  format. If '_PrintNum_Flags_NoUnits' print the formatted number without units.
|
------------------------------------------------------------------------------------------*/

#define _PrintNum_Flags_Raw            _BitM(0)
#define _PrintNum_Flags_NoUnits        _BitM(1)

PUBLIC void compare_PrintNum( S16 n, S_Obj CONST * obj, U8 flags )
{
   S_ObjIO CONST * io;

   io = GetObjIO(_StrConst(obj->name));                                    // get the io format for 'obj' (if it exists)

   if( !io || BSET(flags, _PrintNum_Flags_Raw))                            // no io format OR specified raw print?
   {
      sprintf(PrintBuf.buf, "%i ", n);
   }
   else                                                                    // else print scaled
   {
      sprintf( PrintBuf.buf, _StrConst(io->outFormat), io->outScale * n );    // so print scaled

      if( BCLR(flags, _PrintNum_Flags_NoUnits))                            // append units?
      {
         strcat( PrintBuf.buf, (C8 GENERIC*)_StrConst(io->units) );   // so append units string
      }
   }
   PrintBuffer();
}

/*-----------------------------------------------------------------------------------------
|
|  compare_PrintLimitError()
|
|  Report errors to user
|
------------------------------------------------------------------------------------------*/

#define _LimitError_OK        0
#define _LimitError_Outside   1
#define _LimitError_Above     2
#define _LimitError_Below     3
#define _LimitError_Equal     4
#define _LimitError_NotEqual  5

PRIVATE U8 CONST e1[] = "outside limits  val = ";
PRIVATE U8 CONST e2[] = "over limit  val = ";
PRIVATE U8 CONST e3[] = "under limit  val = ";
PRIVATE U8 CONST e4[] = "not equal  val = ";
PRIVATE U8 CONST e5[] = "equal  val = ";

PRIVATE U8 CONST * CONST msgs[] = {e1, e2, e3, e4, e5};

PRIVATE U8 CONST e11[] = " min = ";
PRIVATE U8 CONST e21[] = " max = ";
PRIVATE U8 CONST e31[] = " compared with ";

PRIVATE U8 CONST * CONST msgs1[] = {e11, e21, e11, e31, e31};

PUBLIC void compare_PrintLimitError( S_Obj CONST * obj, S16 *nums, U8 errNum )
{
   WrStrConst(msgs[errNum-1]);
   compare_PrintNum(nums[0], obj, 0);
   WrStrConst(msgs1[errNum-1]);
   compare_PrintNum(nums[1], obj, _PrintNum_Flags_NoUnits);

   if(errNum == _LimitError_Outside)
   {
      WrStrLiteral("max = ");
      compare_PrintNum(nums[2], obj, _PrintNum_Flags_NoUnits);
   }
   WrStrLiteral("\r\n");
}


// Types of comparision

PUBLIC CONST U8 listOfCompareTypes[] = "inside greater > less < equal == notEqual <> !=";

// *** Duplicated in compare_vec.c
typedef enum  
{  compare_Inside, 
   compare_Greater, compare_GreaterSym, 
   compare_Less, compare_LessSym, 
   compare_Equal, compare_EqualSym, 
   compare_NotEqual, compare_NotEqualSym, compare_NotEqualCSym
} E_Compares;


/*-----------------------------------------------------------------------------------------
|
|  compare_Ints()
|
------------------------------------------------------------------------------------------*/

PUBLIC U8 compare_Ints( S16 *nums, U8 compareType )
{
   BIT eq, less;
   
   eq = nums[0] == nums[1];
   less = nums[0] < nums[1];

   switch( compareType )
   {
      case compare_Inside:
         if( less || (nums[0] > nums[2]) )
            { return _LimitError_Outside; }
         break;

      case compare_Greater:
      case compare_GreaterSym:
         if( less || eq )
            { return _LimitError_Below; }
         break;

      case compare_Less:
      case compare_LessSym:
         if( !less )
            { return _LimitError_Above; }
         break;

      case compare_Equal:
      case compare_EqualSym:
         if( eq == 0 )
            { return _LimitError_NotEqual; }
         break;

      case compare_NotEqual:
      case compare_NotEqualSym:
      case compare_NotEqualCSym:
         if( eq == 1 )
            { return _LimitError_Equal; }
         break;
   }
   return _LimitError_OK;
}

// ------------------------------------- eof ----------------------------------------

