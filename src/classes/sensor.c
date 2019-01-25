/*---------------------------------------------------------------------------
|
|                  Tiny Lib - Sensors
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "sensor.h"
#include "arith.h"

// ============================== PRIVATE =======================================

/*--------------------------------------------------------------------------------
|
|  Sensor_GetCfgFlags()
|
-----------------------------------------------------------------------------------*/

PUBLIC U8 Sensor_GetCfgFlags(S_Sensor RAM_IS *s) { return s->cfg->full.flags; }

#define getCfgFlags Sensor_GetCfgFlags



/*--------------------------------------------------------------------------------
|
|  getCalOrConst()
|
|  Return the current value of a location which may be a S_Cal or a constant.
|
-----------------------------------------------------------------------------------*/

PRIVATE S16 getCalOrConst( S_CalOrConst CONST *k )
{
   if(k->isCal)
      #if USE_NANO_CAL == 1
      { return nCal_Read(k->val.asCal); }
      #else
      { return Cal_Read(k->val.asCal); }
      #endif
   else
      { return k->val.asNum; }
}


/*--------------------------------------------------------------------------------
|
|  Sensor_ReadRaw()
|
|  If S_Sensor 's' has a raw read function, then execute it and return the result,
|  else return 0.
|
|  It's critical to check that the sensor has actually been constructed and hence
|  has a read function. Otherwise the PC resets (jump to zero).
|
-----------------------------------------------------------------------------------*/

PUBLIC S16 Sensor_ReadRaw(S_Sensor RAM_IS *s)
{
   return 
      s->cfg->shrt.read 
         ? (*s->cfg->shrt.read)() 
         : 0;
}

#define readRaw Sensor_ReadRaw


// =========================== end: Private =====================================

/*--------------------------------------------------------------------------------
|
|  Sensor_Init()
|
|  Make S_Sensor 'obj' from S_SensorCfg 'init'. Return 1 if init succeeded
|
-----------------------------------------------------------------------------------*/

PUBLIC U8 Sensor_Init(T_ObjAddr obj, T_CodeAddr init)
{
   ((S_Sensor*)obj)->cfg = (U_SensorCfg CONST*)init;
   ((S_Sensor*)obj)->flags = 0;
   return 1;   
}


#if _TARGET_IS == _TARGET_8051_LIB_SMALL || _TARGET_IS == _TARGET_8051_LIB_IDATA

// ------------------- Sensor_Read() - small models ------------------------------

/*--------------------------------------------------------------------------------
|
|  subtractOfs()
|
|  If there is an offset cal for 's', subtract it from 'n' and return the result.
|  Otherwise leave 'n' unchanged.
|
-----------------------------------------------------------------------------------*/

PRIVATE S16 subtractOfs(S_Sensor RAM_IS *s, S16 n)
{
   return AminusBS16(n, getCalOrConst(&s->cfg->full.scale.ofs));   // so subtract it
}


/*--------------------------------------------------------------------------------
|
|  applyScale()
|
-----------------------------------------------------------------------------------*/

PRIVATE S16 applyScale(S_Sensor RAM_IS *s, S16 n)
{
   return AmulBdivC_S16(n, getCalOrConst(&s->cfg->full.scale.num), getCalOrConst(&s->cfg->full.scale.den));
}

/*--------------------------------------------------------------------------------
|
|  Sensor_Read() - small model
|
|  Return current value of sensor
|
-----------------------------------------------------------------------------------*/

//#pragma AUTO

PUBLIC S16 Sensor_Read(S_Sensor RAM_IS *s)
{
   S16 n;
   
   n = readRaw(s);                           // get the raw value?

   return
   (getCfgFlags(s) == 0)
   ?
      n
   :
      BCLR(getCfgFlags(s), _SensorCfg_SubtractOfsPostGain)
      ?
         applyScale(s, subtractOfs(s, n))
      :
         subtractOfs(s, applyScale(s, n));
}

//#pragma NOAUTO

// ---------------------- end: Sensor_Read() - small models -------------------------

#else

// --------------------- Sensor_Read() - large model --------------------------------

PRIVATE BIT rawIsUnsigned;

/*--------------------------------------------------------------------------------
|
|  legalCalOrConst()
|
|  Return 1 if 'k' contains a legal value, either as a const or a cal
|
-----------------------------------------------------------------------------------*/

PRIVATE BIT legalCalOrConst( S_CalOrConst CONST *k )
{
   if( !k->isCal )                     // holds a const?
      { return 1; }                    // any value is valid
   else                                // else not a const
      { return k->val.asCal != 0; }    // non-zero value assumed to be a valid pointer to and S_Cal
}


/*--------------------------------------------------------------------------------
|
|  subtractOfs()
|
|  If there is an offset cal for 's', subtract it from 'n' and return the result.
|  Otherwise leave 'n' unchanged.
|
-----------------------------------------------------------------------------------*/

PRIVATE S16 subtractOfs(S_Sensor RAM_IS *s, S16 n, U8 readAsUnsigned)
{
   return 
   !legalCalOrConst(&s->cfg->full.scale.ofs)       // Offset not legal?
   ? 
   n                                               // then return 'n' unchanged
   :                                               // else offset is legal   
   ClipS32toS16                                    // so return n - ofs
   ( 
      (
         readAsUnsigned 
         ? 
         (S32)(U16)n 
         : 
         (S32)n 
      ) 
      - 
      getCalOrConst(&s->cfg->full.scale.ofs) 
   );
}

/*--------------------------------------------------------------------------------
|
|  applyScale()
|
-----------------------------------------------------------------------------------*/

PRIVATE S16 applyScale(S_Sensor RAM_IS *s, S16 n)
{
   return
   ClipS32toS16
   ( 
      ( 
         (rawIsUnsigned ? (S32)(U16)n : (S32)n) 
         * 
         (S32)getCalOrConst(&s->cfg->full.scale.num)
      ) 
      / 
      getCalOrConst(&s->cfg->full.scale.den) 
   );
}

/*--------------------------------------------------------------------------------
|
|  Sensor_Read() - large model
|
|  Return current value of sensor.
|
|  This function is made reentrant becasue the read functions of sensors sometimes
|  include other sensors. We hide the reentrantcy in a wrapper rather than making 
|  Sensor_Read() reentrant because the Raisonance 8051 linker LX51 doesn't accept 
|  a reentrant function from a library.
|
-----------------------------------------------------------------------------------*/

PRIVATE S16 Sensor_Read_Reentrant(S_Sensor RAM_IS *s)
{
   S16 n;
   
   n = readRaw(s);                           // get the raw value?
   rawIsUnsigned = BSET(getCfgFlags(s), _SensorCfg_RawIsUnsigned);    // treat raw value as unsigned?

   return
   (getCfgFlags(s) == 0)
   ?
      n
   :
      BCLR(getCfgFlags(s), _SensorCfg_SubtractOfsPostGain)
      ?
         applyScale(s, subtractOfs(s, n, rawIsUnsigned))
      :
         subtractOfs(s, applyScale(s, n), 0);
}

PUBLIC S16 Sensor_Read(S_Sensor RAM_IS *s)
{
   return Sensor_Read_Reentrant(s);
}

// ---------------------- end: Sensor_Read() - large model -------------------------

#endif

// ------------------------------- eof ------------------------------------------
 


