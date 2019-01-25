/* -----------------------------------------------------------------
|
|                    Basic Arithmetic Library
|
|  All these functions are in the 'arith' library.
|
|  Many of these arithmetic functions, such as addition, subtraction and clipping
|  are basic and could be be written inline or defined as macros. However the functions 
|  are better in several ways:
|
|     - The functions are numerically guarded e.g AddTwoS16(int a, int b) will 
|       clip if a + b > 32767. This built-in safety is crucial for the reliable
|       and efficient development of application-specifc numerical algorithmns.
|       The developer is has enough to do figuring the correct algorithmns without
|       having to worry about whether the numerical implementation is reliable.  
|
|     - On smaller MCUs, function calls shrink the code (a lot!). This is especially
|       true for 16 and 32bit arithmetic in 8 bit microcontrollers. The code reused 
|       in the function body is larger than the call overhead. With many calls, the
|       savings really add up.
|
|     - Function calls reduce RAM usage, especially on smaller MCU's, where RAM
|       is scarce. When small arithmetic functions are chained make more complex
|       ones the variable are chained in the registers defined by the compiler's
|       call model. No stack or overlaid RAM. Compilers seem able to figure out
|       small variable allocation problems, but throw up their hands at larger
|       ones. Chaining explicitly directs a compiler to it's own efficient 
|       pre-canned allocations.
|
|     - On any MCU, function calls shrink code because the compiler resolves
|       complex function arguments to a value before passing them to a function.
|       Many functions use arguments more than once; if that function is expressed
|       inline then the compiler may or may not optimise a repreated expression.
|       Sometimes it isn't smart enough to do this, sometimes, as with pointers,
|       or arguments which are themselves function calls  e.g ABS( foo() ), the 
|       compiler must separately evaluate each instance, sometimes e.g ABS(a++), 
|       multiple evaluation is legal but wrong.
|           Whichever way, functions are shorter and safer.
|
--------------------------------------------------------------------- */

#ifndef ARITH_H
#define	ARITH_H

#include "GenericTypeDefs.h"
PUBLIC S16  ClipLongToInt(S32 l);
PUBLIC S16  ClipS32toS16(S32 n);
PUBLIC U16  ClipU32toU16(U32 n);
PUBLIC U8   ClipS16toU8(S16 n);
PUBLIC U8   ClipU16toU8(U16 n);
PUBLIC S8   ClipS16toS8(S16 n);

PUBLIC S8   MinS8(S8 a, S8 b);
PUBLIC S8   MaxS8(S8 a, S8 b);
PUBLIC S8   MaxS8(S8 a, S8 b);
PUBLIC S16  MinS16(S16 a, S16 b);
PUBLIC S16  MaxS16(S16 a, S16 b);

PUBLIC S8   ClipS8(S8 n, S8 min, S8 max);
PUBLIC U8   ClipU8(U8 n, U8 min, U8 max);
PUBLIC U16  ClipU16(U16 n, U16 min, U16 max);
PUBLIC S16  ClipS16(S16 n, S16 min, S16 max);       PUBLIC S16 ClipInt(S16 n, S16 min, S16 max);
PUBLIC S32  ClipS32(S32 n, S32 min, S32 max);
PUBLIC S16  ClipS32_S16Limits(S32 n, S16 min, S16 max);
PUBLIC U8   MinU8(U8 a, U8 b);
PUBLIC U8   MaxU8(U8 a, U8 b);
PUBLIC U16  MinU16(U16 a, U16 b);
PUBLIC U16  MaxU16(U16 a, U16 b);
PUBLIC U32  MinU32(U32 a, U32 b);

PUBLIC BIT  Inside_U8(U8 a, U8 min, U8 max);
PUBLIC BIT  Inside_U16(U16 a, U16 min, U16 max);
PUBLIC BIT  Inside_S16(S16 a, S16 min, S16 max); PUBLIC BIT InsideLimitsS16(S16 n, S16 min, S16 max);

PUBLIC U8   AplusBU8(U8 a, U8 b);
PUBLIC S16  AplusBS16(S16 a, S16 b);
PUBLIC S16  AddTwoS16(S16 a, S16 b);

PUBLIC U32  AplusBU32(U32 a, U32 b);
PUBLIC U16  AplusB_U16(U16 a, U16 b);
PUBLIC S32  AplusBS32(S32 a, S32 b);
PUBLIC S32  AddTwoS32(S32 a, S32 b);


#define AplusBU16(a,b)  AplusB_U16((a),(b))

#define _Inv_AplusB_Inv(a,b)  (1.0 / ((1.0/(a)) + (1.0/(b))) )

PUBLIC U8   AminusBU8(U8 a, U8 b);
PUBLIC U16  AminusBU16(U16 a, U16 b);
PUBLIC S16  AminusBS16(S16 a, S16 b);
PUBLIC S16  AminusBU16ToS16(U16 a, U16 b);

PUBLIC BIT DecrU8_NowZero(U8 * a);
PUBLIC BIT DecrU8_Was1_NowZero(U8 * a);

#define _SignEqualsS16(a,b) ((((a) ^ (b)) & 0x8000) == 0)
PUBLIC BOOL SignEqualsS16(S16 a, S16 b);
PUBLIC S16  AbsS16(S16 n);

PUBLIC S32  MulS16(S16 a, S16 b);
#define    _MulS16ToS16(a,b)  ClipS32toS16((a) * (S32)(b))
PUBLIC S16 MulS16ToS16(S16 a, S16 b);
PUBLIC S16  AmulBdivC_S16(S16 a, S16 b, S16 c);
PUBLIC U16  AmulBdivC_U16(U16 a, U16 b, U16 c);
PUBLIC U16  AmulBdivC_U16_rnd(U16 a, U16 b, U16 c);
PUBLIC S16  AmulBdivC_S16_U8_U8(S16 a, U8 b, U8 c);    PUBLIC S16 AmulBdivC_S16U8(S16 a, U8 b, U8 c);
PUBLIC U8   AmulBdivC_U8(U8 a, U8 b, U8 c);
PUBLIC U8   AmulBdivC_U8_rnd(U8 a, U8 b, U8 c);
PUBLIC S16  AmulBdivC_S16_S8_S8(S16 a, S8 b, S8 c);
PUBLIC S16  AmulBdivC_S16_S8_S8(S16 a, S8 b, S8 c);

PUBLIC U8   MixU8(U8 a, U8 b, U8 m);
PUBLIC U16  MixU16(U16 a, U16 b, U8 m);


PUBLIC U8   Mean2_U8(U8 a, U8 b);
PUBLIC U8   Span3_U8(U8 a, U8 b, U8 c);
PUBLIC U8   Span4_U8(U8 a, U8 b, U8 c, U8 d);
PUBLIC U8   Mean3_U8(U8 a, U8 b, U8 c);
PUBLIC U8   Mean4_U8(U8 a, U8 b, U8 c, U8 d);
PUBLIC U8   AbsDiffU8(U8 a, U8 b);
PUBLIC U16  AbsDiffU16(U16 a, U16 b);
PUBLIC U16  AbsDiffS16(S16 a, S16 b);
PUBLIC U32  AbsDiffU32(U32 a, U32 b);
PUBLIC U8   Median3_U8(U8 a, U8 b, U8 c);

PUBLIC U8   SqrtU16(U16 n);
PUBLIC U16  SqrtU32(U32 n);

PUBLIC S16  DeadbandS16(S16 n, S16 lo, S16 hi);

PUBLIC U16  SqU8(U8 a);
PUBLIC S32  SqS16(S16 n);
PUBLIC U32  SqU16(U16 n);

PUBLIC S32 SqrtLong(S32 n);
PUBLIC U8 SqrtInt(S16 n);
PUBLIC U8 SqrtU16(U16 n);

PUBLIC U8   RootSumSqU8(U8 a, U8 b);
PUBLIC U16  RootSumSqU16(U16 a, U16 b);
PUBLIC U16  RootSumSqS16(S16 a, S16 b);

// Interpolation between 2 values
PUBLIC S16 MixS16(S16 a, S16 b, U8 ratio);         // ratio=0 -> a; ratio = 255 -> b
PUBLIC U8  MixU8(U8 a, U8 b, U8 ratio);
PUBLIC S16 RMS_S16(S16 a, S16 b);                  // Root-mean-square of 'a', 'b'
PUBLIC S16 RootSumSq_S16(S16 a, S16 b);
PUBLIC S16 MixRMS_S16(S16 a, S16 b, U8 ratio);     // Inverse rms, mixed
PUBLIC U8  MixRMS_U8(U8 a, U8 b, U8 ratio);

// Percent difference tests
PUBLIC U8 PcentDiffU8(U8 a, U8 b);                 // Absolute percentage difference
PUBLIC U8 PcentDiffS16(S16 a, S16 b);
PUBLIC BIT WithinPcentU8(U8 a, U8 b, U8 maxPcentApart);
PUBLIC BIT WithinPcentS16(S16 a, S16 b, U8 maxPcentApart);

PUBLIC S8 Ratio10_S16(S16 a, S16 b);      // (a > b) ? (10 *a/b) : (-10*b/a)


// 3-element interpolations; interpolations from a table
PUBLIC S16 Interpolate_NewtonFwd2(S16 y0, S16 y1, S16 y2, U8 n);
PUBLIC S16 ClippedLinSquare(S16 x, S16 linBand, S16 maxOut);
PUBLIC U8 MapToSCurve( S16 n, U16 halfPoint, U8 max );

typedef struct { S16 x, y;} S_XY;

PUBLIC S16 Interpolate2XY( S_XY CONST * tbl, S16 x, U8 tblPoints );
PUBLIC S16 Interpolate2( S16 CONST *tbl, S16 x, U8 tblPoints );
#define _Interpolate2_XScale 256
PUBLIC S16 Linterp_YTbl( S16 CONST *tbl, S16 x, U8 tblPoints );  // Linear interpolation on a fixed-interval Y-table

// Sorts and rank filters

PUBLIC void BubbleSortS16( S16 *v, U8 cnt, U8 ascending );
PUBLIC void BubbleSortU8( U8 *v, U8 cnt, U8 ascending );
#define _BubbleSort_Ascending  1
#define _BubbleSort_Descending 0

#define _Median3(a, b, c) \
   ( ((a)>(b)) ? ( ((b)>(c)) ? (b) : ( ((a)>(c)) ? (c) : (a) ) ) : ( ((a)>(c)) ? (a) : ( ((c)>(b)) ? (b) : (c) ) ) )

PUBLIC S16 Median3_S16(S16 a, S16 b, S16 c);

PUBLIC U8 SigmaSqU8(U8 n);

typedef struct {
    U8 const *theTbl;
    U8 tblSize;
} S_ConstTblU8;

PUBLIC U8 LinterpFromU8Tbl_naked(U8 const *tbl, U8 x);
PUBLIC U8 LinterpFromU8Tbl(S_ConstTblU8 const *tbl, U8 x);
#define _LinterpTblU8_Step        16
#define _LinterpTblU8_MaxEntries  17    // 16 x 16 = 256 (U8) plus end-point

typedef struct {
    S16 const *theTbl;
    U8 tblSize;
} S_ConstTblS16;

PUBLIC S16 LinterpFromS16Tbl(S_ConstTblS16 const *tbl, S16 x);
PUBLIC S16 LinterpFromS16Tbl_naked(S16 const *tbl, S16 x);
#define _LinterpTblS16_Step  256

PUBLIC float GetPwr10Float(S16 exp);
PUBLIC S32   ClipFloatToLong(float f);
PUBLIC S16   ClipFloatToInt(float f);
PUBLIC float MinFloat(float a, float b);
PUBLIC float MaxFloat(float a, float b);
PUBLIC float Mod_Float(float n, float d);
PUBLIC BOOL  InsideEq_Float(float n, float min, float max);

/* ---------------------- Economy Rank Filters ----------------------------
|
|  These rank up to 8 items. 'rank' = 0 returns the largest.
|
---------------------------------------------------------------------------*/

PUBLIC U8 RankU8_UpTo8( U8 const *buf, U8 size, U8 rank );
PUBLIC U8 RankU8_UpTo16( U8 const *buf, U8 size, U8 rank );

PUBLIC U8 MSB_U16(U16 n);
/* ---------------------------- Rank Filter -------------------------------------------

   On each iteration, returns the 'rth' sample, sorted by rank, of the last 'n' samples
   applied to the filter. 'n' must be > 'r'. The filter can be used to return the min, max,
   median, or any rank in between viz:

      r = 0;    returns max
      r = n;    returns min
      r = n/2   returns median

   Rather than sorting existng samples each time a new sample is applied to the buffer
   the filter maintains the samples in a time-linear order and tags each sample with
   its rank. This avoids the inefficient shuffling of data each time a new sample
   is processed.

   When the filter is flushed, it starts with just one sample (of the value supplied
   by _Flush() ). Subsequent samples are added to this one until there are 'n' samples.
   Subsequent new samples displace the oldest ones from the filter, whose length
   remains 'n'.
      While the filter length is less than 'n', a read for the sample of rank 'r'
   return the sample of rank normalised to the current filter length i.e

         r(current) = r(requested) * n(current) / n(final)

   This gives the filter 'fast startup' it will return e.g. the median of the samples
   in the filter so far even before the filter buffer is not yet full.

   The filter will also return the mean and deviation of the samples if requested.
*/

typedef struct {
   S16      value;      /* the data itself                               */
   U8       rank,       /* its current rank among the data in the buffer */
            weight;     /* Hmm! not sure yet                             */
   } rankFilt_S_Entry;

typedef struct {
   rankFilt_S_Entry *start,   /* Rank filter is a circular buffer of 'rankFilt_S_Entry' */
                    *end,
                    *put;
   U8               size,           /* filter buffer size = max length        */
                    sampleCnt;      /* current length of filter, up to 'size' */
   S32              sampleSum;      /* used for mean and deviation            */
   } rankFilt_S;


PUBLIC void        rankFilt_Init  ( rankFilt_S *f,  rankFilt_S_Entry *buf, U8 size  );
PUBLIC void        rankFilt_Flush ( rankFilt_S *f,  S16 initValue              );
PUBLIC BOOL        rankFilt_Run   ( rankFilt_S *f,  S16 in                     );
PUBLIC S16         rankFilt_Read  ( rankFilt_S *f,  U8 targetRank                   );
PUBLIC S16         rankFilt_Dev   ( rankFilt_S *f                                      );
#define rankFilt_Mean(f)  ((f)->sampleSum / (f)->sampleCnt)

/* ------------------------- end: Rank Filter ------------------------------------------ */

#ifndef PI
    #define PI 3.14159265358979
#endif

#define SQRT2 1.41421356237

// ---------------------- For constant expressions ---------------------------------
//
// These macros solve problems particular to constant expressions. They will work
// at runtime but they're really not for that.

// Return the floor() and fractional parts of a (floating point) number
// Uses '%' to block promotion to float within the sub-expression.
#define _Floor_Const(n) ((((long)(n) - 1) % (long)(n) ) + 1)
#define _Frac_Const(n)  ((n) - _Floor_Const(n))

/* --------------------- Scalar Accumulator ------------------------------------- */

typedef struct
{
   S32 acc;
   U16 cnt;
} S_Acc;

PUBLIC void Acc_Add(S_Acc *a, S16 n);
PUBLIC void Acc_Clear(S_Acc *a);
PUBLIC S16  Acc_Read(S_Acc *a);
PUBLIC S16  Acc_ReadAndClear(S_Acc *a);
#define _Acc_Cnt(a)  ((a).cnt)

/* --------------- Scalar Accumulator which maintains deviation sum --------------- */

typedef struct
{
   S32 acc;
   U16 cnt;
   S32 devSum;
} S_AccDev;

PUBLIC void AccDev_Add(S_AccDev *a, S16 n);
PUBLIC void AccDev_Clear(S_AccDev *a);
PUBLIC S16  AccDev_Read(S_AccDev *a);
PUBLIC S16  AccDev_MeanDev(S_AccDev *a);
PUBLIC S16 AccDev_DevSum(S_AccDev *a);
PUBLIC S16  AccDev_ReadAndClear(S_AccDev *a);
#define _AccDev_Cnt(a)  ((a).cnt)

/* ---------------------- Basic Vectors / unsized --------------------------------------

   Note, these are different the the Vec_ library which use the more versatile but
   larger sized-vector 'S_Vec'.
*/

PUBLIC S16 VecB_Span(S16 *v, U8 cnt);
PUBLIC U8 VecB_Centroid(S16 *v, U8 cnt);
PUBLIC S16 VecB_Max(S16 *v, U8 cnt);
PUBLIC U8 VecB_MaxIdx(S16 *v, U8 cnt);       // Index to max value (rather than value itself)
PUBLIC S16 VecB_MeanDiff(S16 *v1, S16 *v2, U8 cnt);
PUBLIC S16 VecB_MaxAbsDiff(S16 const *v1, S16 const *v2, U8 cnt);
PUBLIC S16 VecB_LargestDiff(S16 const *v1, S16 const *v2, U8 cnt);
PUBLIC S16 VecB_Min(S16 *v, U8 cnt);
PUBLIC S16 VecB_Mean(S16 *v, U8 cnt);
PUBLIC void VecB_ByFraction(S16 *v, U8 cnt, U8 mul, U8 div);
PUBLIC S32 VecB_Sum(S16 *v, U8 cnt);
PUBLIC S16 VecB_SumToS16(S16 *v, U8 cnt);       // Sum limited to MAX_S16
PUBLIC void VecB_ClipSame(S16 *v, S16 min, S16 max, U8 cnt);  // Clip each element between 'min' and 'max'

// --------------------------- Vector Decimator ----------------------------------------

typedef struct
{
   S16 *in, *out;
   S32 *acc;
   U8  size, div;
} S_VecDecimatorCfg;

typedef struct
{
   S_VecDecimatorCfg CONST *cfg;
   U8  cnt;
} S_VecDecimator;

PUBLIC void VecDecimator_Init(S_VecDecimator *d, S_VecDecimatorCfg CONST *cfg);
PUBLIC BIT VecDecimator_Run(S_VecDecimator *d);


/* ---------------------- Vectors Operations on Bytes ------------------------ */

PUBLIC U8  VecU8_Max(U8 *v, U8 cnt);
PUBLIC S16 VecU8_Sum(U8 *v, U8 cnt);
PUBLIC U8  VecU8_Mean(U8 *v, U8 cnt);
PUBLIC U8  VecU8_Dev(U8 *v, U8 cnt);
PUBLIC S16 VecU8_DevSum(U8 *v, U8 cnt);


/* ----------------------- Floating Point Specialty --------------------------- */

PUBLIC float GetPwr10Float(S16 exp);

#endif	/* ARITH_H */

// -------------------------------- eof -----------------------------------

