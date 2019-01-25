/*---------------------------------------------------------------------------
|
|                    Test Fixture Support - Calibration Vectors
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "calv.h"
#include "flashblk.h"      // FlashBlk_Read()
         // sprintf
#include "hostcomms.h"     // Comms_WrStr()
#include "sysobj.h"        // T_ObjAddr, T_CodeAddr 
#include "links.h"         // Link_SenderChanged()
#include "arith.h"         // ClipS16()
#include "vec.h"           // Vec_Read1


// ============================== PRIVATE =====================================

/*-----------------------------------------------------------------------------------
|
|  CalV_GetVec()
|
--------------------------------------------------------------------------------------*/

PUBLIC S_Vec * CalV_GetVec( S_CalV *cal )
{
   return cal->vec;
}
#define getVec CalV_GetVec

PRIVATE U8 getVecSize( S_CalV *cal )
{
   return getVec(cal)->rows * getVec(cal)->cols;
}

PRIVATE U8 getVecBytes( S_CalV *cal )
{
   return sizeof(S16) * getVecSize(cal);
}

PUBLIC S16 CalV_GetMin( S_CalV *cal )
{
   return cal->cfg->cfg.min;
}
#define getMin CalV_GetMin

PUBLIC S16 CalV_GetMax( S_CalV *cal )
{
   return cal->cfg->cfg.max;
}
#define getMax CalV_GetMax

// ============================ end: Private ======================================



/*-----------------------------------------------------------------------------------
|
|  CalV_Init()
|
--------------------------------------------------------------------------------------*/

PUBLIC U8 CalV_Init( T_ObjAddr obj, T_CodeAddr init)
{
   S_Obj CONST *vec;

   if( !(vec = GetObj_C(((S_CalVCfg CONST *)init)->vecName)) )    // try to get the current value vector. fail?
   {
      return 0;                                                   // then return construct fail
   }
   else                                                           // else got the vector
   {
      ((S_CalV*)obj)->vec = (S_Vec*)vec->addr;                    // so load vector addr into the S_CalV
      ((S_CalV*)obj)->cfg = (S_CalVCfg CONST *)init;              // load ptr to cfg

      if( !CalV_Recall( (S_CalV*)obj))                            // try to recall stored value. fail?
      { 
         CalV_ToDefault( (S_CalV*)obj ); 

         sprintf((char*)PrintBuf.buf, "No stored cal for: %s\r\n", GetObjName(obj) );
         PrintBuffer();
      }

      ((S_CalV*)obj)->valid = 1;
      return 1;                                                   // return construct success!
   }
}


/*-----------------------------------------------------------------------------------
|
|  CalV_Read1()
|
--------------------------------------------------------------------------------------*/

PUBLIC S16 CalV_Read1( S_CalV *cal, T_VecRows row, T_VecCols col )
{
   return Vec_Read1( (S_Vec const *)getVec(cal), row, col );   // $$Z8
}


/*-----------------------------------------------------------------------------------
|
|  CalV_ReadStored()
|
|  Return 1 if read was successful, else 0.
|
--------------------------------------------------------------------------------------*/

PUBLIC U8 CalV_ReadStored( S_CalV *cal, S16 *out )
{
   U8 rtn;

   rtn = FlashBlk_Read( (U8 *)out, cal->cfg->cfg.storeHdl, getVecBytes(cal) );
   return rtn;
}


/*-----------------------------------------------------------------------------------
|
|  CalV_GotStored()
|
|  Return 1 if there is a stored value for the cal.
|
--------------------------------------------------------------------------------------*/

PUBLIC U8 CalV_GotStored( S_CalV *cal )
{
   return FlashBlk_VarExists( cal->cfg->cfg.storeHdl );
}


/*-----------------------------------------------------------------------------------
|
|  CalV_Write1()
|
--------------------------------------------------------------------------------------*/

PUBLIC void CalV_Write1( S_CalV *cal, S16 n, T_VecRows row, T_VecCols col )
{
   Vec_Write1( getVec(cal), ClipS16(n, getMin(cal), getMax(cal)), row, col);
   Link_SenderChanged((T_ObjAddr)cal);
}


/*-----------------------------------------------------------------------------------
|
|  CalV_ToDefault()
|
--------------------------------------------------------------------------------------*/

PUBLIC void CalV_ToDefault( S_CalV *cal )
{
   Vec_Fill( getVec(cal), cal->cfg->cfg.dflt);
   Link_SenderChanged((T_ObjAddr)cal);
}


/*-----------------------------------------------------------------------------------
|
|  CalV_Store()
|
--------------------------------------------------------------------------------------*/

PUBLIC U8 CalV_Store( S_CalV *cal )
{
   return FlashBlk_Write( (U8*)getVec(cal)->nums, cal->cfg->cfg.storeHdl, getVecBytes(cal) );
}


/*-----------------------------------------------------------------------------------
|
|  CalV_Recall()
|
--------------------------------------------------------------------------------------*/

PUBLIC U8 CalV_Recall( S_CalV *cal )
{
   U8 rtn;

   rtn = FlashBlk_Read( (U8 *)(getVec(cal)->nums), cal->cfg->cfg.storeHdl, getVecBytes(cal) );
   Link_SenderChanged((T_ObjAddr)cal);
   return rtn;
}


// ------------------------------- eof ---------------------------------------

