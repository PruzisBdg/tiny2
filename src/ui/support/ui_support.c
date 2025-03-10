/*---------------------------------------------------------------------------
|
|                Tiny Lib - Text UI Support
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "sysobj.h"
#include "class.h"
#include "tty_ui.h"
#include "wordlist.h"
#include "calv.h"
#include "tiny1util.h"
#include "arith.h"
#include "romstring.h"
#include "hostcomms.h"



/*--------------------------------------------------------------------------------
|
|  Scanf_NoArgs()
|
|  ANSI is ambiguous about the return from scanf() when the source string is empty.
|  Depending on vendor library; sscanf() may return either 0 or EOF if there are no args.
|
|
-----------------------------------------------------------------------------------*/

PUBLIC U8 Scanf_NoArgs(U8 scanfRtn )
{
   /* !!!! Cast EOF (-1) to U8 so that it becomes 0xFF, to match 'scanfRtn'. Otherwise
      this comparision will fail in a compiler with full promotion enabled.
   */
   return scanfRtn == 0 || scanfRtn == (U8)EOF;
}

/*--------------------------------------------------------------------------------
|
|  UI_GetAnyObj_MsgIfFail()
|
-----------------------------------------------------------------------------------*/

PUBLIC S_Obj CONST * UI_GetAnyObj_MsgIfFail(C8 *args)
{
   #define _MaxChars 15

   S_Obj CONST * obj;
   C8 firstArg[_MaxChars+1];

   if( NULL != (obj = GetObj(args)) )
   {
      return obj;
   }
   else
   {
      Str_CopyNthWord(args, firstArg, 0, _MaxChars);      // get 1st arg, which should have been an object
      sprintf(PrintBuf.buf, _StrLit("\'%s\' is not an object\r\n   Type \'objs\' to list all objects by type"), firstArg);
      PrintBuffer();
      return 0;
   }
   #undef _MaxChars
}


/*--------------------------------------------------------------------------------
|
|  UI_GetObj_MsgIfFail()
|
-----------------------------------------------------------------------------------*/

PUBLIC S_Obj CONST * UI_GetObj_MsgIfFail(C8 *args, U8 classID)
{
   #define _MaxChars 15

   S_Obj CONST * obj;
   C8 firstArg[_MaxChars+1];

   if( NULL != (obj = GetObjByClass(args, classID)) )
   {
      return obj;
   }
   else
   {
      Str_CopyNthWord(args, firstArg, 0, _MaxChars);      // get 1st arg, which should have been an object
      sprintf(PrintBuf.buf, _StrLit("\'%s\' is not a %s!\r\n   Type \'objs\' to list all objects by type"), firstArg, _StrConst(GetClassName(classID)));
      PrintBuffer();
      return 0;
   }
   #undef _MaxChars
}

/*--------------------------------------------------------------------------------
|
|  UI_GetAction_MsgIfFail()
|
-----------------------------------------------------------------------------------*/

PUBLIC U8 UI_GetAction_MsgIfFail(C8 *args, C8 CONST * actionList)
{
   U8 action;

   if( (action = Str_FindWord(_StrConst(actionList), Str_GetNthWord(args, 1))) == _Str_NoMatch )
   {
      sprintf(PrintBuf.buf, _StrLit("Action must be one of: \'%s\'\r\n   Type \'help <cmd>\' for more info\r\n"), _StrConst(actionList));
      PrintBuffer();
      return 0xFF;
   }
   else
   {
      return action;
   }

}



/*-----------------------------------------------------------------------------------------
|
|  UI_PrintObject()
|
|  Print the value of a single object
|
------------------------------------------------------------------------------------------*/

PUBLIC void UI_PrintObject( S_Obj CONST *obj, U8 flags )
{
   S_ObjIO CONST * io;
   BIT  appendVecUnits;

   if(BSET(flags, _UI_PrintObject_Raw))
      { io = 0; }
   else
      { io = GetObjIO(_StrConst(obj->name)); }

   if(BSET(flags, _UI_PrintObject_PrependName))
   {
      sprintf(PrintBuf.buf, "%s = ", _StrConst(obj->name));
      PrintBuffer();
   }

   // Append units to a vector only if there's an IO spec AND flags are set to print units
   appendVecUnits = io && BSET(flags, _UI_PrintObject_AppendUnits);

   switch(obj->type)
   {
      case _Class_Vec:
         UI_PrintVector( (S_Vec *)obj->addr, io, appendVecUnits );
         break;

      case _Class_CalVec:
         UI_PrintVector( ((S_CalV *)obj->addr)->vec, io, appendVecUnits );
         break;

      default:
         if( Obj_IsAScalar(obj) )
            { UI_PrintScalar( Obj_ReadScalar(obj), io, flags & _UI_PrintObject_AppendUnits ); }
         else
            { WrStrLiteral("Can't read this\r\n"); }
   }
}



/*-----------------------------------------------------------------------------------------
|
|  UI_RawInArgList()
|
|  Return 1 if 'args' contains "raw", else 0.
|
------------------------------------------------------------------------------------------*/

PUBLIC BIT UI_RawInArgList(C8 * args)
{
   return Str_WordInStr( args, "raw");
}


/*-----------------------------------------------------------------------------------------
|
|  UI_IntFromArgs()
|
|  Fetch the number from position 'idx' in argument list 'args'. If 'io' is non-zero then
|  divide the number by io->outScale. 'idx' == 0 references the 1st argument.
|
|  PARAMETERS:
|
|  RETURNS:
|
------------------------------------------------------------------------------------------*/

PUBLIC S16 UI_IntFromArgs( C8 *args, S_ObjIO CONST *io, U8 idx)
{
   float f;
   C8 *p;

   p = Str_GetNthWord(args, idx);

   if( !sscanf(p, "%f", &f) )    // not a number?
   {
      sprintf(PrintBuf.buf, _StrLit("arg %d is not a number\r\n"), idx);
      PrintBuffer();
      return 0;                                          // polite return
   }
   else                                                  // else got a number
   {
      if( io ) { f /= io->outScale; }                    // if scaling exists then apply scaling
      return ClipFloatToInt(f);                          // clip result into integer.
   }
}


/*-----------------------------------------------------------------------------------------
|
|  skipUnaryPrefix()
|
|  Skip 'p' past the '-' (minus) or '!' (not) which might be in front of an object name.
|
------------------------------------------------------------------------------------------*/

PUBLIC C8 * skipUnaryPrefix(C8 * p)
{
   return *p == '-' || *p == '!' ? p+1 : p;
}

/*-----------------------------------------------------------------------------------------
|
|  GetArgIO()
|
|  If the 1st argument in args is a scalar object, returns the (IO) formatter of that object.
|  If the 1st arg isn't and object or doesn't have a scale, then returns 0
|
------------------------------------------------------------------------------------------*/

PUBLIC S_ObjIO CONST * GetArgIO(C8 *args)
{
   S_ObjIO CONST * io;

   return
      ( (io = GetObjIO(_StrConst(GetObj(skipUnaryPrefix(args))->name))) != 0)    // This Vector or scalar has an IO scale
      && !UI_RawInArgList(args)                                                  // AND 'raw' was NOT in the args list
      && *args != '!'                                                            // AND the object name was NOT prefixed by '!' (not)
         ? io                                                                    // then return the io spec that we retrieved above
         : 0;                                                                    // else return 0
}


/*-----------------------------------------------------------------------------------------
|
|  GetArgScale()
|
|  If the 1st argument in args is a scalar object, returns the (IO) scale of that object.
|  If the 1st arg isn't and object or doesn't have a scale, then returns 1.0
|
------------------------------------------------------------------------------------------*/

PUBLIC float GetArgScale(C8 *args)
{

   S_ObjIO CONST * io;

   return
      (io = GetArgIO(args)) == 0    // Args have no scale OR 'raw' specified?
         ? 1.0                      // the return scale = 1.0
         : io->outScale;            // else return the scale implied by the args
}







// ----------------------------- eof -----------------------------------------
