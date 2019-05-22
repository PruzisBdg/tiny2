/*---------------------------------------------------------------------------
|
|                          Fixed block-size queue
|
|--------------------------------------------------------------------------*/

/* ------------------------------ bQ_Write ----------------------------------------

   Push '*msg' onto head of 'q'. A whole block of 'q->dataSize' bytes is copied to 'q'.

   Returns: TRUE if the block was written
            FALSE if the queue could not be acquired or was full.
*/

#include "libs_support.h"
#include "bq.h"

extern void bQ_CopyBytes( U8 bq_RAMSPACE *src, U8 bq_RAMSPACE *dest, U8 cnt );


PUBLIC BOOLEAN bQ_Push( bq_S bq_MEMSPACE *q, U8 bq_IOSPACE *msg )
{
   /* no room? */
   if( q->cnt >= q->qSize )
   {
      return FALSE;
   }
   /* room - write to queue. */
   else
   {
      /* This is a push() so first back up 'get'. */
      q->get -= q->blockStep;

      if( q->get < q->start )          // Before start of buffer?
      {
         q->get = q->end;              // then backup to start of last data slot.
      }
      bQ_CopyBytes( msg, q->get, q->dataSize );

      q->cnt++;

      return TRUE;
   }
}

// --------------------------------- eof -----------------------------------
