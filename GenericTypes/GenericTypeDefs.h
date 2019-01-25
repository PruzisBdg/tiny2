/*******************************************************************

                  Generic Type Definitions

*******************************************************************/

#ifndef __GENERIC_TYPE_DEFS_H1_
#define __GENERIC_TYPE_DEFS_H1_

/* Specify an extension for GCC based compilers */
#if defined(__GNUC__)
#define __EXTENSION __extension__
#else
#define __EXTENSION
#endif

#if !defined(__PACKED)
    #define __PACKED
#endif

#undef TRUE
#undef FALSE

typedef enum _BOOL { FALSE = 0, TRUE } BOOL;    /* Undefined size */
typedef enum _BIT { CLEAR = 0, SET } BIT;

#define _ToBool(n) ((n) == 0 ? FALSE : TRUE)

#define PUBLIC                                  /* Function attributes */
#define PROTECTED
#define PRIVATE static

typedef signed short int S16;                      // Succinct versions of the above
typedef signed long int S32;
#define _S16(n)  ((S16)(n))

/* MPLAB C Compiler for PIC18 does not support 64-bit integers */
#if !defined(__18CXX)
__EXTENSION typedef signed long long    INT64;
#endif

typedef unsigned char  U8;                      // Succinct versions of the above
typedef unsigned short int U16;
#define _U8(n)  ((U8)(n))
#define _U16(n)  ((U16)(n))

typedef signed char S8;
typedef char C8;

// Extrema
#define MIN_U8 0
#define MAX_U8 0xFF
#define MIN_S8 (-128)
#define MAX_S8 127
#define MIN_C8 0
#define MAX_C8 127

#define MIN_U16 0
#define MAX_U16 0xFFFF
#define MAX_S16 32767
#define MIN_S16 (-32768)

#define MIN_U32 0
#define MAX_U32 0xFFFFFFFF

#define MAX_S32  ((S32)( 2147483647LL))
#define MIN_S32  ((S32)(-2147483648LL))

#define HIGH_BYTE(n) ((U8)((n) >> 8))
#define LOW_BYTE(n)  ((U8)((n) & 0xFF))

#define LOW_NIBBLE(b)  ((U8)((b) & 0x0F))
#define HIGH_NIBBLE(b)   ((U8)(((b) >> 4) & 0x0F))

#define HIGH_WORD(n) ((U16)((n) >> 16))
#define LOW_WORD(n)  ((U16)((n) & 0xFFFF))

#define HIGH_DWORD(n) ((U32)((n) >> 32))
#define LOW_DWORD(n)  ((U32)((n) & 0xFFFFFFFF))


/* 24-bit type only available on C18 */
#if defined(__18CXX)
typedef unsigned short long UINT24;
#endif
//typedef unsigned long int   UINT32;     /* other name for 32-bit integer */
typedef unsigned long int U32;                     // Succinct.

/* MPLAB C Compiler for PIC18 does not support 64-bit integers */
#if !defined(__18CXX)
__EXTENSION typedef unsigned long long  UINT64;
#endif


#define RECORDS_IN(a)  (sizeof(a)/sizeof(a[0]))

#define MAX(a,b)  ((a) > (b) ? (a) : (b))
#define MIN(a,b)  ((a) < (b) ? (a) : (b))
#define ABS(n)    ((n) < 0 ? -(n) : (n))

#define CONST const

#define PRIVATE static
#define PUBLIC

/* ------------------- Token Pasting --------------------------------------

   When a token paste is used directly in a macro, an argument is not checked
   for further expansion before being pasted (unlike when the argument is
   concatentated). Embedding the paste itself in macro allows expansion prior
   to pasting.
*/
#define CAT(x,y)  x##y
#define XCAT(x,y)  CAT(x,y)

// --------------------------------Static assert. ------------------------------
//
// Makes an enum  with name which is unique within a file. Does not reserve storage,
// so does not need to be NO_DEBUG-ed.

//#ifdef ASSERT
//    #undef ASSERT
//#endif

#define ASSERT(condition) enum { XCAT(_compile_assert_var_at_line_,  __LINE__) = 1/(condition) };

// This array-definition version works only with integer expressions. Non-integers can't be
// arguments for an array size, even if the result is TRUE (1). Most compilers complain.
// #define ASSERT(condition) typedef struct{char assert_failure[((condition) ? 1 : -1)];} XCAT(S_, __LINE__);

#endif /* __GENERIC_TYPE_DEFS_H_ */
