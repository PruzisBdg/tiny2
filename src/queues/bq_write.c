/*---------------------------------------------------------------------------
|
|                          Fixed block-size queue
|
|--------------------------------------------------------------------------*/

/* ------------------------------ bQ_Write ----------------------------------------

   Write '*msg' to 'q'. A whole block of 'q->dataSize' bytes is copied to 'q'.

   Returns: TRUE if the block was written
            FALSE if the queue could not be acquired or was full.
*/

#include "libs_support.h"
#include "bq.h"

extern void bQ_CopyBytes( U8 bq_RAMSPACE *src, U8 bq_RAMSPACE *dest, U8 cnt );


PUBLIC BOOLEAN bQ_Write( bq_S bq_MEMSPACE *q, U8 bq_IOSPACE *msg )
{

   /* no room? */
   if( q->cnt >= q->qSize )
   {
      return FALSE;
   }
   /* room - write to queue. */
   else
   {
      bQ_CopyBytes( msg, q->put, q->dataSize );

      /* Bump put ptr and wrap? */
      q->put += q->blockStep;

      if( q->put > q->end )
         q->put = q->start;

      q->cnt++;

      return TRUE;
   }
}

// --------------------------------- eof -----------------------------------
