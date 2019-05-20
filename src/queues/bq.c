/*---------------------------------------------------------------------------
|
|                          Fixed block-size queue
|
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
|--------------------------------------------------------------------------*/


#include "libs_support.h"
#include "bq.h"

/* ------------------------------ Constructor -----------------------------------

   Return FALSE if couldn't allocate RAM for the queue.
*/

PUBLIC BOOLEAN bQ_Init( bq_S  bq_MEMSPACE  *bq,          // Make this
                        U8    bq_RAMSPACE  *ram,         // using this RAM
                        U8                 qSize,        // this many blocks
                        U8                 dataSize )    // bytes per block
{
   if(qSize == 0 || dataSize == 0)     /* Sizes legal? */
   {
      return FALSE;                    /* Fail if not */
   }
   else
   {
      bq->qSize = qSize;
      bq->dataSize = dataSize;

      bq->start = ram;
      bq->end = bq->start + ((U16)(bq->qSize-1) * bq->dataSize);     /* mark start of last slot */
      bQ_Flush(bq);                                                  /* 'put', 'get' set to queue start; 'cnt' = 0 */
      return TRUE;
   }
}


/* ---------------------------- Private functions ------------------------------- */

PRIVATE void flushQueue( bq_S bq_MEMSPACE *q )
{
   q->cnt = 0;
   q->put = q->start;
   q->get = q->start;
   return;
}

PUBLIC void bQ_CopyBytes( U8 bq_RAMSPACE *src, U8 bq_RAMSPACE *dest, U8 cnt )
{
   for( ; cnt; cnt--, src++, dest++ ) *dest = *src;
}



/* ---------------------------- bQ_Read --------------------------------------

   Get into next block from 'q' into 'msg'. 'q->dataSize' bytes are written
   into 'msg'.

   Returns: the number of bytes read.
            0 if queue was empty OR queue was locked.

*/
PUBLIC bQ_T_MsgSize bQ_Read( bq_S bq_MEMSPACE *q, U8 bq_IOSPACE *msg ) {

      /* empty? */
   if( !q->cnt )
   {
      return 0;
   }

   /* otherwise read the next item. */
   else
   {
      /* If there's a 'msg' to copy to then copy out 'get'. Otherwise just remove 'get'
         from the queue (i.e Take())
      */
      if(msg != NULL)
      {
         bQ_CopyBytes( q->get, msg, q->dataSize);
      }

      /* Bump get ptr and wrap? */
      q->get += q->dataSize;

      if( q->get > q->end )
         q->get = q->start;

      q->cnt--;

      return q->dataSize;
   }
}


/* -------------------------- bQ_Flush -------------------------------------

   Set 'put' and 'get' ptrs to start of queue; 'cnt' to zero.

   Returns TRUE if the queue was flushed; FALSE if the queue was locked.
*/

PUBLIC BOOLEAN bQ_Flush( bq_S bq_MEMSPACE *q )
{
   flushQueue(q);
   return TRUE;
}


/* =================================== eof ================================= */
