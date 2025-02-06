/*---------------------------------------------------------------------------
|
|   Evaluates unary, binary and ternary arithmetic and logical expressions
|
|  See 'C:\SPJ1\swr\tiny1\TOS1 Guide rev.nn.pdf'
|
|--------------------------------------------------------------------------*/


#include "common.h"
#include "wordlist.h"
#include "sysobj.h"
#include "class.h"
#include "tiny1util.h"
#include "arith.h"
#include "tty_ui.h"

#include "romstring.h"
#include "util.h"

// Operator list; the 1st is a dummy (null) value
typedef enum
{
   op_Null,
   op_Assign, op_AssignPlus, op_AssignMinus, op_AssignMul,
   op_Plus, op_Minus,
   op_Equal, op_Neq, op_Greater, op_GreaterEq, op_Less, op_LessEq,
   op_Mul, op_Div,
   op_And, op_Or,
   op_Min, op_Max
} E_Operators;
PRIVATE C8 CONST operators[] = "n = += -= *= + - == != > >= < <= * / && || min max";

#define _OpIsAssignment(op)  ((op) >= op_Assign && (op) <= op_AssignMul)

/*-----------------------------------------------------------------------------------------
|
|  isComparision()
|
|  Returns 1 if 'op' is an arithmetic comparision e.g > (greater)
|
------------------------------------------------------------------------------------------*/

PRIVATE BIT isComparision(U8 op)
{
   return op >= op_Equal && op <= op_LessEq;
}


/*-----------------------------------------------------------------------------------------
|
|  resultIsBoolean()
|
|  Returns 1 if 'op' returns a boolean.
|
------------------------------------------------------------------------------------------*/

PRIVATE BIT resultIsBoolean(U8 op)
{
   return isComparision(op) || op == op_And || op == op_Or;
}




extern float GetArgScale(C8 *args);

/*-----------------------------------------------------------------------------------------
|
|  eval2()
|
|  Returns result of 'left' 'operator' 'right'  e.g 4 - 1 returns 3. If 'operator' is
|  a logical then returns 1 if results is true, else 0
|
|  For arithmetic operators eval2() clips the results into an integer (S16).
|
|  If the operator is not recognised then returns 0.
|
------------------------------------------------------------------------------------------*/

PRIVATE S16 eval2(S16 left, S16 right, U8 operator)
{
   switch( operator )
   {
      case op_Plus:
         return AplusBS16(left, right);

      case op_Minus:
         return AminusBS16(left, right);

      case op_Equal:
         return left == right;

      case op_Neq:
         return left != right;

      case op_Greater:
         return left > right;

      case op_GreaterEq:
         return left >= right;

      case op_Less:
         return left < right;

      case op_LessEq:
         return left <= right;

      case op_Mul:
         return MulS16ToS16(left, right);

      case op_Div:
         return left / right;

      case op_And:
         return left && right;

      case op_Or:
         return left || right;

      case op_Min:
         return MinS16(left, right);

      case op_Max:
         return MaxS16(left, right);

      default:
         return 0;
   }
}


/*-----------------------------------------------------------------------------------------
|
|  binaryExprScale()
|
|  Return scaling for binary expression 'expr' of the for 'arg1 operator arg2'.
|
|  If either of 'arg1' or 'arg2' are objects and if either has a scale then the value
|  returned will be that scale. If not then returns scale = 1.0
|
------------------------------------------------------------------------------------------*/

PRIVATE float binaryExprScale(C8 *expr)
{
   return
      GetArgScale(                           // Return the scale of the the arg...
         Str_GetNthWord(                     // which is the nth word of ...
            expr,                            // 'expr', and 'nth' is..
            GetArgScale(expr) == 1.0         // If the scale of the 1st arg is 1.0
               ? 2                           // the 3rd word of 'expr' (i.e the 2nd arg)
               : 0 ));                       // else the 1st word of 'expr' (i.e the 1st arg)
}


/*-----------------------------------------------------------------------------------------
|
|  getBinaryOperator()
|
|  Search 'expr' from the 2nd token onward and return the first binary operator found.
|  Returns one of 'E_Operators'. If no operator found then returns 'op_Null' = 0.
|
------------------------------------------------------------------------------------------*/

PRIVATE U8 getBinaryOperator(C8 *expr)
{
   U8 op;

   if( (op = Str_FindWord(_StrConst(operators), Str_GetNthWord(expr,1))) == _Str_NoMatch)
      { return op_Null; }
   else
      { return op; }
}

/*-----------------------------------------------------------------------------------------
|
|  GetExprIO()
|
|  Return IO formatter for binary expression 'expr' of the for 'arg1 operator arg2'.
|
------------------------------------------------------------------------------------------*/

extern S_ObjIO CONST * GetArgIO(C8 *args);

PRIVATE S_ObjIO CONST * GetExprIO(C8 *expr)
{
   S_ObjIO CONST * io;
   U8 c;

   if( resultIsBoolean( getBinaryOperator(expr) ))          // Operator returns boolean?
      { return 0; }                                         // then result is logical (unscaled)
   else                                                     // else examine the operands
   {
      for(c = 0; c <= 4; c += 2)                            // For each operand, 0th, 2nd, 4th args
      {
         if( (io = GetArgIO(Str_GetNthWord(expr,c))) )   // This operand has an IO spec?
            { return io; }                                  // then return with this IO spec.
      }
      return 0;
   }
}


/*-----------------------------------------------------------------------------------------
|
|  mulDiv2Float()
|
|  Return a * b if operator is 'multiply', else a / b.
|
|  This handles FP operations where at least one arg is a literal.
|
------------------------------------------------------------------------------------------*/

PRIVATE S16 mulDiv2Float(float a, float b, U8 operator)
{
   return
      ClipFloatToInt(
         operator == op_Mul
            ? a * b
            : a / b );

}


/*-----------------------------------------------------------------------------------------
|
|  evalExpr2()
|
|  Evaluates binary expression string of the form "arg1 operator arg2 .... [raw]" OR
|  evaluates just the values of a variable or literal "arg1"
|
|  If one of arg1 or arg2 is a variable with an IO scale and the other arg is a literal
|  number then the literal is scaled to the variable UNLESS 'raw' is part of 'expr'.
|  E.g with voltages scaled as mV
|     "SupplyV > 13.1V"   or "SupplyV > 13100 raw" are equivalent
|
|  arg1, arg2 can be variables with two different scales but their internal (integer) values
|  are compared directly; there is no rescaling.
|
|  An empty expression always returns false (0).
|
|  If 'expr' isn't legal then returns 0.
|
|
------------------------------------------------------------------------------------------*/

PRIVATE S16 evalExpr2(C8 *expr, float scale)
{
   U8 operator, argCnt;
   float s1, s2;

   if( (argCnt = Str_WordCnt(expr)) == 0 )            // empty?
   {
      return 0;                                       // then return 0 (false)
   }
   else if( argCnt == 1 )                             // just one arg?
   {
      return UI_GetScalarArg(expr, 0, scale);         // then read it
   }
   else                                               // else assume its a binary expression
   {                                                  // read the operator (the 2nd word)
      if( !(operator = getBinaryOperator(expr)) )    // operator not legal?
      {
         return 0;                                    // then return 0
      }
      if(operator == op_Mul || operator == op_Div)    // multiplication or division?
      {

         if( !ReadASCIIToFloat(expr, &s1 ) )
         {
            if( !ReadASCIIToFloat(Str_GetNthWord(expr, 2), &s2) )
               { return 0; }
            else
               { s1 = Obj_ReadScalar(GetObj(expr)); }
         }
         else
         {
            if( !ReadASCIIToFloat(Str_GetNthWord(expr, 2), &s2) )
               { s2 = Obj_ReadScalar(GetObj(Str_GetNthWord(expr, 2))); }
         }
         return mulDiv2Float(s1, s2, operator);
      }
      else                                            // else not multiplication or division
      {
         if( binaryExprScale(expr) != 1.0 )
            { scale = binaryExprScale(expr); }                  // get expression scale if one implied by the variables

         return
            eval2(
               UI_GetScalarArg(expr, 0, scale),
               UI_GetScalarArg(expr, 2, scale),
               operator);                 // return result.
      }
   }
}

/*-----------------------------------------------------------------------------------------
|
|  evalExpr2SelfScale()
|
------------------------------------------------------------------------------------------*/

PRIVATE S16 evalExpr2SelfScale(C8 *expr)
{
   return evalExpr2(expr, 1.0);
}



/*-----------------------------------------------------------------------------------------
|
|  EvalExpr()
|
|  Evaluates an expression, returns the result in the internal units implied by the
|  arguments.
|
------------------------------------------------------------------------------------------*/

PUBLIC S16 EvalExpr(C8 *expr)
{
   U8 op1, op2;
   float scale, f1, f2;
   S_Obj CONST *obj;
   S16 n;

   if( Str_WordCnt(expr) == 0 )                          // An empty expression?
   {
      return 0;                                          // An empty expression always returns false
   }

   if( !(op1 = getBinaryOperator(expr)) )                // no legal operator?
   {
      return UI_GetScalarArg(expr, 0, 1.0);              // then evaluate 1st arg only
   }

   if( _OpIsAssignment(op1) )                            // 1st arg is assignment?
   {
      if( (obj = GetObj(expr)) == 0 )                    // 1st arg isn't an object? (it must be)
         { return 0; }
      else if( !Obj_IsWritableScalar(obj) )              // 1st arg isn't writable? (it must be)
         { return 0; }
      else                                               // else 1st arg is valid l-value
      {
         n = evalExpr2(                                  // Get the 2nd arg, which is the eval of...
               Str_GetNthWord(expr,2),                   // ...the remainder of the expression
               GetArgScale(expr));

         if(op1 == op_AssignPlus)                        // '+=" ?
            {  n = AplusBS16(Obj_ReadScalar(obj), n); }  // the l-value is incremented by 2nd arg
         else if(op1 == op_AssignMinus)                  // '-='?
            {  n = AminusBS16(Obj_ReadScalar(obj), n); } // then l-value is decremented by arg
         else if(op1 == op_AssignMul)                    // '*='?
            {  n = MulS16ToS16(Obj_ReadScalar(obj), n); } // then l-value is multipled by arg
         else                                            // else '='
            {}                                           // so 'l-value' will be set to 2nd arg
         Obj_WriteScalar(obj, n);                        // write the l-value (Scalar)
         return n;                                       // and return the resulting value (of the modified l-value)
      }
   }
   else                                                  // else 1st arg isn't assignment
   {
      if( !(op2 = getBinaryOperator(Str_GetNthWord(expr,2)))  )  // No 2nd operator?
      {
         return evalExpr2SelfScale(expr);                    // then its a binary expression - evaluate it
      }
      else                                                  // else its ternary expression
      {
         if( resultIsBoolean(op1) )                         // 1st operator returns a boolean? ...
         {                                                  // then must evaluate right-to-left
            scale =                                         // Scale for the whole expression is....
               GetObj(expr) != 0                            // If 1st arg is an object
                  ? GetArgScale(expr)                       // then that object determines the scale
                  : binaryExprScale(Str_GetNthWord(expr,2));    // else remaining 2 terms determine the scale

            return                                          // Return the ternary expression result which is....
               eval2(                                       // the binary combination of...
                  UI_GetScalarArg(expr, 0, scale),     // the value of the 1st arg
                  evalExpr2(                                // with the result of binary combination of
                     Str_GetNthWord(expr,2),                // the 2nd, 3rd args with their operator
                     scale),
                     op1);                                  // all combined using the 1st operator
         }
         else                                               // else 1st arg is arithmetic
         {                                                  // so evaluate left-to-right
            if(op2 == op_Mul || op2 == op_Div)              // Multiply or divide?
            {
               if( ReadASCIIToFloat(Str_GetNthWord(expr, 4), &f1) )   // 3rd arg is a literal number
               {

                  /* ****** COMPILER BUG

                     This expression gives error
                        mulDiv2Float(evalExpr2SelfScale(expr), f1, op2))
                     evaluates instead to
                        mulDiv2Float(evalExpr2SelfScale(expr), evalExpr2SelfScale(expr), op2))
                     i.e the 1st parm is passed twice.

                     Workaround is 'f2 = evalExpr2SelfScale(expr)' then
                     mulDiv2Float(f2, f1, op2)
                  */
                  f2 = evalExpr2SelfScale(expr);
                  return mulDiv2Float(f2, f1, op2);  // then mul/divide this 3rd by result of 1st and 2nd
               }
               else
               {
                  return eval2(evalExpr2SelfScale(expr), UI_GetScalarArg(expr, 4, 1.0), op2);
               }
            }
            else
            {
               return
                  eval2(
                     evalExpr2SelfScale(expr),
                     UI_GetScalarArg(
                        expr,
                        4,
                        isComparision(op2)               // Scale to apply to 3rd arg is.... If 2nd arg is a comparsion?
                           ? 1.0                         // then result is unscaled
                           : binaryExprScale(expr)),     // else it's scale flows from the 1st 2 args
                        op2);
            }
         }
      }
   }
}

/*-----------------------------------------------------------------------------------------
|
|  UI_Eval()
|
|  Evaluates an expression, prints the result.
|
------------------------------------------------------------------------------------------*/

PUBLIC U8 CONST Eval_Help[] = "\
Usage: eval <expr>  prints ""= expression_value""\r\n\
   highest  =(assigment)   == <> > < >= <=    + - * / &(and) |(or) lowest\r\n\
\r\n\
   3 + 67   = 70\r\n\
   8 / 2    = 4\r\n\
   8 / 2.1  = 4 (integer aritmetic)\r\n\
   SupplyV > 10  = 1 (true)\r\n\
   v1 = 20  = 20 (v1 is set to 20)\r\n\
   v1 = v2 - 3\r\n\
";

PUBLIC U8 UI_Eval( C8 *args )
{
   UI_PrintScalar(EvalExpr(args), GetExprIO(args), _UI_PrintScalar_AppendUnits );
   return 1;
}

// ----------------------------------- eof ------------------------------------------------
