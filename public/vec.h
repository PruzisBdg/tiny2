/*---------------------------------------------------------------------
|
|                    2 - Dimensional Integer Vectors
|   
|  Public functions are ...
|
|     Copies and Fills
|        CopyBytes         a memcpy()
|        FillBytes
|
|     Vectors
|        Vec_MakeMultiple - Make the vectors listed in a S_VecCfg
|        S_Vec          - a vector (matrix); has rows and cols
|        
|        Vec_Read1      - access 1 element in a vector
|        Vec_Write1     - write one vector element
|        vec_Write      - write one vector onto another, clipping the source vector if necessary
|        Vec_ByElement  - run unary or binary operation (passed as func. ptr) on
|                         each vector(s) element
|        Vec_Copy
|        Vec_Add
|        Vec_Subtract
|        Vec_ByFraction - v[n] = num * v[n] / den
|        Vec_AddScalar  - add/subtract scalar to every element 
|        Vec_Fill       - set each element to same value
|        Vec_RefRow     - point to given row in a vector
|        Vec_Ref        - Access part of a row
|        Vec_MinMax_Idx - Get index to min or max element
|        Vec_Sub        - Extract (copy out) a sub-vector
|        Vec_Join       - Join / merge 2 vectors, extending the destination vector if necessary
|        Vec_Match      - Find (idx of) nearest match to target value
|        Vec_Linterp    - linear interpolation of vector onto a new axis
|        Vec_BubbleSort - ascending or descending
|
|        Vec_SumRow     - sum of 1st row
|        Vec_Mean       - mean of 1st row
|        Vec_StdDev     - sample (N-1) deviation
|        Vec_RMS        - root mean square
|        Vec_MeanAbs    - mean of absolute value
|
|        Vec_Sequence   - Make a linear sequence
|
|        Vec_Peaks      - Find any of maxs, mins or zero-crossings on a vector
|        Vec_WrapString - Wrap a line 'string' around a curve
|        
|
|   
| 
|---------------------------------------------------------------------------*/

#ifndef VEC_H
#define VEC_H

#include "common.h"      /* common defnitions */

/* '_cdecl' is required when included in C++ files */
#ifdef _DECL_EXTERN_C
   #define _DECL(ret)   extern "C" ret _cdecl
#else
   #define _DECL(ret)   PUBLIC ret
#endif


/* ---------------------------- Copies and Fills ------------------------------ */

_DECL(void) CopyBytes( U8 *dest, U8 const *src, U16 cnt );
_DECL(void) FillBytes( U8 *dest, U8 n, U16 cnt );

/*#define _CopyInts(dest, src, cnt)  CopyBytes((U8*)(dest), (U8*)(src), 2 * (cnt)) */
/*#define _FillInts(dest, n, cnt)    FillBytes((U8*)(dest), (n),        2 * (cnt)) */

_DECL(void) CopyInts( S16 *dest, S16 const *src, U16 cnt );
_DECL(void) FillInts( S16 *dest, S16 n, U16 cnt );

_DECL(S32) SqrtLong(S32 n);

/* -------------------------- Vector / Matrix ------------------------------- 

   May have multiple row and columns
*/

typedef U8 T_VecRows;
#define Vec_MaxRows MAX_UCHAR
#define _Vec_LAST_ROW  MAX_UCHAR

#ifdef _TARGET_IS_PC                /* At present PC-based BP apps require large-column vectors */
   typedef U16 T_VecCols;
   typedef U16 T_VecRowOrCol;
   #define Vec_MaxCols MAX_UINT
   #define _Vec_LAST_COL  MAX_UINT
   typedef U32 T_RowByCol;
#else
   typedef U8 T_VecCols;
   typedef U8 T_VecRowOrCol;
   #define Vec_MaxCols MAX_UCHAR
   #define _Vec_LAST_COL  MAX_UCHAR
   typedef U16 T_RowByCol;
#endif


typedef struct       // A vector
{
   T_VecRows  rows;
   T_VecCols  cols;
   S16        *nums;
} S_Vec;

typedef struct       // specifies a vector; used by Vec_MakeMultiple()
{
   S_Vec      *vec;
   S16        *buf;
   T_VecRows  rows;
   T_VecCols  cols;
} S_VecCfg;


typedef struct       // A long vector
{
   T_VecRows  rows;
   T_VecCols  cols;
   S32        *nums;
} S_LVec;


/* ---------------------------- Vectors ------------------------------------------- */

#define _Vec_Make(v, buf, r, c)  \
   (v)->nums = (buf);            \
   (v)->rows = (r);              \
   (v)->cols = (c);
   
#define _Vec_Buf(v)  ((v)->nums)   
#define _Vec_Rows(v)  ((v)->rows)   
#define _Vec_Cols(v)  ((v)->cols)
#define _Vec_LastRow(v) ((v)->rows - 1)   
#define _Vec_LastCol(v) ((v)->cols - 1)

_DECL(void) Vec_MakeMultiple( S_VecCfg CONST *l, U8 numVecs );

_DECL(T_VecRowOrCol) Vec_Size(S_Vec const *v);

#define _Vec_Idx(v,r,c) (((v)->cols * (T_RowByCol)(r)) + (c))     // Given r,c, makes index into v->nums[] 

_DECL(S16)  Vec_Read1( S_Vec const *v, T_VecRows row, T_VecCols col );
#define _Vec_Read1(v, row, col) ((v)->nums[((v)->cols * (T_RowByCol)(row)) + (col)])   /* faster, no protection */
_DECL(void) Vec_Write1( S_Vec *v, S16 n, T_VecRows row, T_VecCols col );
_DECL(void) Vec_Write( S_Vec *dest, S_Vec const *src, T_VecRows startRow, T_VecCols startCol );
#define _Vec_Write1(v,n,row,col)  (v)->nums[((v)->cols * (T_RowByCol)(row)) + (col)] = (n)
_DECL(void) Vec_ByElement( S_Vec *dest, S_Vec const *src, void(*op)(S16*, S16*, S16), S16 parm );
_DECL(void) Vec_ByElementT( S_Vec *dest, S_Vec const *src, void(*op)(S16*, S16*, S16), S16 parm );
_DECL(void) Vec_Transpose( S_Vec *mainVec, S_Vec *ioVec, T_VecRows startRow, T_VecCols startCol, U8 insert );
_DECL(void) Vec_Transpose1( S_Vec *v );
_DECL(void) Vec_ByElementL( S_LVec *lvec, S_Vec *svec, void(*op)(S32*, S16*, S16), S16 parm );
#define _Vec_Tranpose_Extract 0
#define _Vec_Tranpose_Insert 1

_DECL(void)      Vec_Copy( S_Vec *dest, S_Vec const *src );
_DECL(void)      Vec_Fill( S_Vec *dest, S16 n );
_DECL(T_VecCols) Vec_MinMax_Idx( S_Vec const *vec, U8 ctl );
_DECL(void)      Vec_RefRow( S_Vec *sub, S_Vec const *in, T_VecRows row );
_DECL(void)      Vec_Ref( S_Vec *sub, S_Vec const *ref, T_VecRows row, T_VecCols startCol, T_VecCols numCols );

typedef struct
{
   T_VecRows startRow;
   T_VecCols startCol;
   T_VecRows numRows;
   T_VecCols numCols;      
} S_Vec_SubCtl;

_DECL(void) Vec_Join( S_Vec *dest, S_Vec const *src, T_VecRows startRow, T_VecCols startCol );
_DECL(void) Vec_Sub( S_Vec *sub, S_Vec const *in, S_Vec_SubCtl const *ctl );
_DECL(void) Vec_Add( S_Vec *dest, S_Vec const *src );
_DECL(void) Vec_AddClip( S_Vec *dest, S_Vec const *src );
_DECL(void) Vec_Subtract( S_Vec *dest, S_Vec const *src );
_DECL(void) Vec_Diff( S_Vec *dest, S_Vec const *src );
_DECL(void) Vec_Linterp(S_Vec *out, S_Vec const *in, S_Vec const *axis );
_DECL(void) Vec_ByFraction( S_Vec *dest, S_Vec const *src, S16 num, S16 den );
_DECL(void) Vec_AddScalar( S_Vec *dest, S_Vec const *src, S16 n );

PUBLIC S16 Vec_RMS( S_Vec const *v );
_DECL(S16)  Vec_Normalise( S_Vec const *v, S_Vec *vn, S16 center, S16 size, U8 ctl );

#define _Vec_Sort_Ascending TRUE
#define _Vec_Sort_Descending FLASE
_DECL(void) Vec_BubbleSort( S_Vec *v, BOOL ascending );


#define _Vec_Ctl_GetMin   _BitM(0) 
#define _Vec_Ctl_GetMax   _BitM(1) 

#define _Vec_MinIdx(v)  (Vec_MinMax_Idx((v), _Vec_Ctl_GetMin))
#define _Vec_MaxIdx(v)  (Vec_MinMax_Idx((v), _Vec_Ctl_GetMax))

#define _Vec_Max(v)  ( (v)->nums[Vec_MinMax_Idx((v), _Vec_Ctl_GetMax)] )
#define _Vec_Min(v)  ( (v)->nums[Vec_MinMax_Idx((v), _Vec_Ctl_GetMin)] )

_DECL(S16) Vec_Min(S_Vec const *v);
_DECL(S16) Vec_Max(S_Vec const *v);
_DECL(S16) Vec_Span(S_Vec const *v);

_DECL(void) Vec_Extents( S_Vec *e, S_Vec const *v, U8 ctl );
#define _Vec_Extents_Accumulate _BitM(0)        /* min/max of 'e' and 'v'                          */
#define _Vec_Extents_NoAccumulate 0             /* (else 'e' <- min/max of 'v' only)               */

#define _Vec_Extents_AddColIdx _BitM(1)         /* Row 0 of 'e' are the min/max col indices of 'v' */




_DECL(S16) Vec_Match( S_Vec const * v, S16 y, U8 ctl );

#define _Vec_Match_SrchUp     _BitM(0)    /* Search up indices; return 1st match                */
#define _Vec_Match_SrchDown   _BitM(1)    /* Search down indices; return 1st match              */
                                          /* else will find nearest globally                    */

#define _Vec_Match_Inside     _BitM(2)    /* For search up/down, return 1st point before match  */
#define _Vec_Match_Outside    _BitM(3)    /* or 1st point after                                 */
#define _Vec_Match_Nearest    0           /* else return nearest of 2 points which straddle     */

#define _Vec_Match_RampsUp    _BitM(4)    /* Tell Vec_Match that vector is monotonically increasing      */
#define _Vec_Match_RampsDown  _BitM(5)    /* else tell that is is monotonically decreasing               */
                                          /* else Vec_Match compares end-points of vector to find slope  */

#define _Vec_Match_OntoRow0   _BitM(6)    /* For 2-row vectors, sets 2nd row (row 1) as the domain for   */
                                          /* interpolation, with result interpolated onto row 0. Default */
                                          /* is domain in row 0, interpolated onto row 1                 */
#define _Vec_Match_OntoRow1   0

#define _Vec_Match_NoInterpolate _BitM(7)  /* Just return the index of match, even if there are two rows  */                                           

/* ---- Statistics */
_DECL(S32) Vec_SumRow( S_Vec const *v );
_DECL(S16) Vec_Mean( S_Vec const *v );
_DECL(S16) Vec_MeanAbs( S_Vec const *v );
_DECL(S16) Vec_StdDev( S_Vec const *v );
_DECL(S16) Vec_RowCOG( S_Vec const *v );


_DECL(void) Vec_Sequence( S_Vec *v, S16 start, S16 step, S16 cnt );

/* Peak location */
#define _Vec_Peaks_Maxs          _BitM(0)       /* Get maxima        */
#define _Vec_Peaks_Mins          _BitM(1)       /* Get minima        */
#define _Vec_Peaks_Zeros         _BitM(2)       /* Get zero-cross    */
#define _Vec_Peaks_Rank          _BitM(3)       /* Rank max/mins     */
#define _Vec_Peaks_Indices       _BitM(4)       /* For a multi-row source vector, return both indices and x-vals of mathing peaks */
#define _Vec_Peaks_XValues       _BitM(5)

#define _Vec_Peaks_exType_None     0
#define _Vec_Peaks_exType_Max      1
#define _Vec_Peaks_exType_Min      2
#define _Vec_Peaks_exType_ZeroUp   3
#define _Vec_Peaks_exType_ZeroDown 4

_DECL(S16) Vec_Peaks( S_Vec *out, S_Vec const *in, U8 ctl );

#define _Vec_WrapString_Over     0
#define _Vec_WrapString_Under    _BitM(1)


_DECL(void) Vec_WrapString( S_Vec *string, S_Vec const *in, U8 ctl );

/* --------------------------- end: Vectors -------------------------------------- */


_DECL(S16) Linterp_XY_1( S_Vec const *t, S16 x );
_DECL(S16) Vec_Linterp_XYTable( S_Vec const *t, S16 x );


#define LVAR               // Local vars in 8051 idata
#define _HAS_FLOAT 1       // Has floating point support

#endif /* VEC_H -------------------- eof --------------------------- */
