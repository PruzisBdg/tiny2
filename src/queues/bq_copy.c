/*---------------------------------------------------------------------------
|
|                          Fixed block-size queue
|
|--------------------------------------------------------------------------*/

#include "libs_support.h"
#include "bq.h"

/* ---------------------------- bQ_Copy --------------------------------------

   Copy head of 'q' into '*msg' without removing it. 'q->dataSize' bytes are written
   into 'msg'.

   Returns: the number of bytes copied into 'msg'.
            0 if queue was empty OR queue was locked.

*/

extern void bQ_CopyBytes( U8 bq_RAMSPACE *src, U8 bq_RAMSPACE *dest, U8 cnt );

PUBLIC bQ_T_MsgSize bQ_Copy( bq_S bq_MEMSPACE *q, U8 bq_IOSPACE *msg )
{
   /* empty? */
   if( !q->cnt )
   {
      return 0;
   }

   /* otherwise copy out the head item. */
   else
   {
      bQ_CopyBytes( q->get, msg, q->dataSize);
      return q->dataSize;
   }
}


// --------------------------------- eof -----------------------------------
