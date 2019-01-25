/* -------------------------------------------------------------------------
|
|                             Utility Library
|
|  All thse functions are in the 'util' library.
|
----------------------------------------------------------------------------- */

#ifndef UTIL_H
#define UTIL_H

#include "libs_support.h"

/* ------------------------- On Ints ------------------------------- */

PUBLIC void FillIntsU8( S16 RAM_IS *a, S16 n, U8 cnt );
PUBLIC void CopyIntsU8( S16 RAM_IS *dest, S16 const  *src, U8 cnt );
PUBLIC void CopyConstIntsU8( S16 RAM_IS *dest, S16 CONST *src, U8 cnt );
PUBLIC S32 SumIntsU8( S16 RAM_IS *a, U8 cnt );

/* ------------------------- On bytes -------------------------------- */

PUBLIC void CopyBytesU8( U8 RAM_IS *src, U8 RAM_IS *dest, U8 cnt );
PUBLIC void CopyConstBytesU8( U8 CONST *src, U8 RAM_IS *dest, U8 cnt );
PUBLIC void FillBytesU8( U8  RAM_IS *a, U8 n, U8 cnt );
PUBLIC void ZeroBytesU8(void RAM_IS *p, U8 cnt);
PUBLIC U16 SumBytesU8( U8 RAM_IS *a, U8 cnt );
PUBLIC void SwapBytesU8(void RAM_IS *a, void RAM_IS *b, U8 cnt);

/* ------------------------- On floats -------------------------------- */

PUBLIC void FillFloatsU8( float RAM_IS *a, float n, U8 cnt );

/* ------------------------- Hex ASCII -------------------------------- */

PUBLIC BIT  IsHexASCII(U8 ch);
PUBLIC U8   HexToNibble(U8 ch);

PUBLIC U8   LowNibbleToHexASCII(U8 n);
PUBLIC U8   HighNibbleToHexASCII(U8 n);
PUBLIC U8   NibbleToHexASCII(U8 n);

// Macro versions of the above, for constants
#define _NibbleToHexASCII(n)  ((n) < 10 ? (n) + '0' : ((n) <= 0x0F ? (n)-10+'A' : 0))
#define _HighNibbleToHexASCII(n)  _NibbleToHexASCII( HIGH_NIBBLE((n)) )
#define _LowNibbleToHexASCII(n)  _NibbleToHexASCII( LOW_NIBBLE((n)) )

// To construct HexASCII strings
PUBLIC void ByteToHexASCII(U8 *hex, U8 n);
PUBLIC U8   HexASCIItoByte(U8 const *hex);
PUBLIC void IntToHexASCII_Space(U8 *out, S16 n);
PUBLIC void IntToHexASCII_NoSpace(U8 *out, S16 n);

// To parse HexASCII strings
PUBLIC U8 const* GetNextHexASCIIByte(U8 const *hexStr, U8 *out);
PUBLIC U8 const* GetNextHexASCIIInt(U8 const *hexStr, S16 *out);


// -------------------------- ACSII to Number Parsers ------------------------

PUBLIC U8 const * ReadASCIIToFloat(U8 const *inTxt, float *out);
PUBLIC BIT        GotFloatFromASCII(U8 const *inTxt, float *out); 
PUBLIC U8 const * ReadDirtyASCIIInt(U8 const *inTxt, S16 *out);
PUBLIC U8 const * ReadDirtyBinaryWord(U8 const *inTxt, U16 *out); 


// Is fed one char at a time.
PUBLIC void ReadDirtyASCIIInt_ByCh_Init(void);
PUBLIC BIT  ReadDirtyASCIIInt_ByCh(U8 ch, S16 *out);

/* ----------------------- Basic byte queue ------------------------------- */

typedef struct {
   U8 *buf,
       size,
       put,
       get,
       cnt,
       locked;
} S_byteQ;

PUBLIC void byteQ_Init  ( S_byteQ *q, U8 *buf, U8 size);
PUBLIC void byteQ_Flush ( S_byteQ *q);
PUBLIC BIT  byteQ_Write ( S_byteQ *q, U8 const *src, U8 bytesToWrite);
PUBLIC BIT  byteQ_Read  ( S_byteQ *q, U8 *dest,      U8 bytesToRead);
PUBLIC BIT  byteQ_Locked( S_byteQ *q);
PUBLIC U8   byteQ_Count ( S_byteQ *q);


// Wrapper for a for() loop.
PUBLIC void RepeatIdxFunc( void( *func)(U8), U8 repeatCnt);


/* ---------------------------- Bits ---------------------------------- */

PUBLIC U8 NumBitsSet_U16(U16 n);
PUBLIC U8 IdxTo1stBitSet_U16(U16 n);

PUBLIC U8 NumBitsSet_U32(U32 n);
PUBLIC U8 IdxTo1stBitSet_U32(U32 n);

/* --------------------- Time/Date, ISO8601 format ------------------------- 

   These functions convert between seconds and hours/minutes/seconds (HMS) and
   date/time (YMD/HMS). They can render HMS and YMD/HMS as ISO8601 strings.

   Zero seconds is midnight, New-years Eve 2000AD and spans forward 132 years to
   2132AD.
*/

/* A 32bit count of seconds. Will count 132 years */
typedef U32 T_Seconds32;

typedef struct {
   U16 yr;           // 0000 - 9999.  0000 = 1.BC 0001 = 1.AD
   U8  mnth,         // 1 - 12 = Jan -> Dec
       day,          // 1 - 31
       hr,           // 0 - 23
       min,sec;      // 0 - 59
   } S_DateTime;

/* Used when exporting this struct (to another device). Then the contents are packed, regardless 
   of the format internal to the code.
*/
#define _YMDHMS_BytesPacked 7


// Up to 9999 hrs when rendered as an ISO8601 string i.e max is "9999:59:59".
typedef struct {
   U16 hr;           // 0 - 9999
   U8 min, sec;      // 0 - 59
   } S_TimeHMS;

/* Used when exporting this struct (to another device). Then the contents are packed, regardless 
   of the format internal to the code.
*/
#define _HMS_BytesPacked 4

#define _2000AD 2000    // 32bit seconds are offset from midnight, New-years eve 2000AD.

PUBLIC void          SecsToHMS(T_Seconds32 secs, S_TimeHMS *hms);
PUBLIC U8            SecsToHMSStr(T_Seconds32 secsCnt, U8 *strOut);
PUBLIC void          SecsToYMDHMS(T_Seconds32 secsSince2000AD, S_DateTime *dt);
PUBLIC void          SecsTo_YMDHMS_Str(T_Seconds32 secs, U8 *strOut );
PUBLIC BOOLEAN       ISO8601StrToSecs( U8 const *dateStr, T_Seconds32 *absTimeOut );
PUBLIC T_Seconds32   YMDHMS_To_Secs(S_DateTime const *dt);
PUBLIC BOOLEAN       Legal_YMDHMS(S_DateTime const *t);
PUBLIC U8            YMDHMS_ToStr(S_DateTime const *t, U8 *outStr);
extern S16 const     DaysToMonthStartTbl[];

/* -------------------------- Table search ----------------------------------------- */

PUBLIC void const * TblSearchSAR_AscendKeyU16(void const *tbl, U16 numRecords, U8 bytesPerRec, U16 keyToMatch);

PUBLIC U8* EndStr(U8* str);  // Spot the end of a non-const string.

#endif // UTIL_H 
