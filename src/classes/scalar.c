/*---------------------------------------------------------------------------
|
|              Tiny1 - Scalar Class
|
|  A scalar is just an integer 'S16'.
|
|--------------------------------------------------------------------------*/

#include "common.h"

PUBLIC U8 Scalar_Init( S16 *s, S16 initVal )
{
   *s = initVal;
   return 1;
}

PUBLIC U16 Scalar_Read( S16 *s )
{
   return *s;
}

PUBLIC void Scalar_Write( S16 *s, S16 n )
{
   *s = n;
}

// -------------------------- eof ---------------------------------------


 
