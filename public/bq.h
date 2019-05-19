/*---------------------------------------------------------------------------
|                             Integrated Bench
|
|                    Framework Extensions - Block Queues
|
|  Interlocked queues for fixed length data blocks.
|
|  Limits:
|
|     Data block size:              255 bytes
|     Queue capacity (blocks):      255
|
|  Storage for a 'BlockQueue' is allocated when the queue is created.
|
|  Queues are intelocked. Currently operation on a 'BlockQueue' are executed
|  at the 'user' privelidge level of the parent 'Active' class.
|--------------------------------------------------------------------------*/

#ifndef BQ_H
#define BQ_H

#include "common.h"

#define bQ_T_MsgCnt  U8             /* up to 255 messages                                 */
#define bQ_T_MsgSize U8             /* of up to 255 bytes                                 */

#ifndef _TARGET_IS
   #error "_TARGET_IS must be defined"
#else

   #if _TARGET_IS == _TARGET_8051_LIB_SMALL
      #define bq_MEMSPACE  IDATA          /* for the queue itself                               */
      #define bq_RAMSPACE  IDATA          /* for RAM used by the queue                          */
      #define bq_rRAMSPACE IDATA          /* register version - for Z8                          */
      #define bq_IOSPACE   IDATA          /* to read and write to the queue                     */
   #else
      #define bq_MEMSPACE                 /* for the queue itself                               */
      #define bq_RAMSPACE                 /* for RAM used by the queue                          */
      #define bq_rRAMSPACE                /* register version - for Z8                          */
      #define bq_IOSPACE                  /* to read and write to the queue                     */
   #endif
#endif

typedef struct
{
   bQ_T_MsgCnt        cnt;          /* number of messages in queue                        */
   bQ_T_MsgCnt        qSize;        /* in messages (from Cfg)                             */
   bQ_T_MsgSize       dataSize;     /* bytes per message (from Cfg)                       */
   U8 bq_RAMSPACE     *put;         /* start of the next free slot for a message          */
   U8 bq_RAMSPACE     *get;         /* start of the oldest message                        */
   U8 bq_RAMSPACE     *start;       /* start of the 1st slot in the buffer for a message  */
   U8 bq_RAMSPACE     *end;         /* START of the last slot in the buffer.              */
} bq_S;


/* ------------------------------ Interface ---------------------------------

   Note that normally the creator/user of a bQ_ will known the queue block size so,
   for bQ_Read(), there is no strict need to return the number of bytes read; a
   TRUE/FALSE would do. However the uint return is used to allow bQ_'s to be used
   interchangeably with variable length queues ( VQueues, vq.c ).
*/

PUBLIC BOOLEAN      bQ_Init(  bq_S bq_MEMSPACE *bq, U8 bq_RAMSPACE *ram, U8 qSize, U8 dataSize );
PUBLIC BOOLEAN      bQ_Flush( bq_S bq_MEMSPACE *q );
PUBLIC BOOLEAN      bQ_Write( bq_S bq_MEMSPACE *q, U8 bq_IOSPACE *msg );
PUBLIC BOOLEAN      bQ_Push( bq_S bq_MEMSPACE *q, U8 bq_IOSPACE *msg );
PUBLIC bQ_T_MsgSize bQ_Peek( bq_S bq_MEMSPACE *q, U8 bq_IOSPACE *msg );
PUBLIC bQ_T_MsgSize bQ_Read(  bq_S bq_MEMSPACE *q, U8 bq_IOSPACE *msg );
#define             bQ_Count(q)  ((q)->cnt)
PUBLIC bQ_T_MsgCnt  bQ_Free( bq_S bq_MEMSPACE *q );

/* Inline version of bQ_Write(). bQ_ funtions are not reentrant so use this in
   interrupts.

   For 8051 put the local variables for the block-copy into DATA, the fastest
   memory space. This copy will be used in interrupt so we want it as quick as
   possible. Cost is just 5 bytes.
*/
   #if _TOOL_IS == TOOL_RIDE_8051

#define _bQ_RWrite_INLINE_Decl         \
   U8 bq_RAMSPACE * DATA src;          \
   U8 bq_RAMSPACE * DATA dest;         \
   U8               DATA cnt;

#define _bQ_Write_INLINE(q, srcBuf)    \
                                       \
   if( (q).cnt < (q).qSize )           \
   {                                   \
      for( cnt = (q).dataSize,         \
           src = (U8*)(srcBuf),        \
           dest = (q).put;             \
           cnt;                        \
           cnt--,                      \
           src++, dest++ )             \
         { *dest = *src; }             \
                                       \
      (q).put += (q).dataSize;         \
                                       \
      if( (q).put > (q).end )          \
         (q).put = (q).start;          \
                                       \
      (q).cnt++;                       \
   }

#define _bQ_Read_INLINE(q, destBuf)    \
                                       \
   if( (q).cnt > 0 )                   \
   {                                   \
      for( cnt = (q).dataSize,         \
           dest = (U8*)(destBuf),      \
           src = (q).get;              \
           cnt;                        \
           cnt--,                      \
           src++, dest++ )             \
         { *dest = *src; }             \
                                       \
      (q).get += (q).dataSize;         \
                                       \
      if( (q).get > (q).end )          \
         (q).get = (q).start;          \
                                       \
      (q).cnt--;                       \
   }

   #else  // _TOOL_IS == TOOL_RIDE_8051

#define _bQ_RWrite_INLINE_Decl    \
   U8 bq_RAMSPACE *src, *dest;    \
   U8 rIDATA cnt;

#define _bQ_Write_INLINE(q, srcBuf)    \
                                       \
   if( (q).cnt < (q).qSize )           \
   {                                   \
      for( cnt = (q).dataSize,         \
           src = (U8*)(srcBuf),        \
           dest = (q).put;             \
           cnt;                        \
           cnt--,                      \
           src++, dest++ )             \
         { *dest = *src; }             \
                                       \
      (q).put += (q).dataSize;         \
                                       \
      if( (q).put > (q).end )          \
         (q).put = (q).start;          \
                                       \
      (q).cnt++;                       \
   }

#define _bQ_Read_INLINE(q, destBuf)    \
                                       \
   if( (q).cnt > 0 )                   \
   {                                   \
      for( cnt = (q).dataSize,         \
           dest = (U8*)(destBuf),      \
           src = (q).get;              \
           cnt;                        \
           cnt--,                      \
           src++, dest++ )             \
         { *dest = *src; }             \
                                       \
      (q).get += (q).dataSize;         \
                                       \
      if( (q).get > (q).end )          \
         (q).get = (q).start;          \
                                       \
      (q).cnt--;                       \
   }

   #endif  // _TOOL_IS == TOOL_RIDE_8051

// Inline functions for 16 bits systems
// User must ensure that buffers are (half)word aligned.

#define _bQ_RWrite_INLINE_ByWords_Decl    \
   U16 bq_RAMSPACE *src, *dest;           \
   U8 rIDATA cnt;

#define _bQ_Write_INLINE_ByWords(q, srcBuf)    \
                                       \
   if( (q).cnt < (q).qSize )           \
   {                                   \
      for( cnt = (q).dataSize/2,       \
           src = (U16*)(srcBuf),       \
           dest = (U16*)(q).put;       \
           cnt;                        \
           cnt--,                      \
           src++, dest++ )             \
         { *dest = *src; }             \
                                       \
      (q).put += (q).dataSize;         \
                                       \
      if( (q).put > (q).end )          \
         (q).put = (q).start;          \
                                       \
      (q).cnt++;                       \
   }


#endif /* BQ_H */

