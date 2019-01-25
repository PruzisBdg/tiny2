/*---------------------------------------------------------------------------
|
|              Test Fixture Support - Class Definitions
|
|  This source file is to be included in each project. The switches in 'bldcfg.h"
|  for the project determine which classes are linked in.
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "sysobj.h"
#include "class.h"
#include "romstring.h"

extern U8 Sensor_Init(T_ObjAddr obj, T_CodeAddr init);
extern U8 Vec_Init(T_ObjAddr obj, T_CodeAddr init);
extern U8 Scalar_Init(T_ObjAddr obj, T_CodeAddr init);
extern U8 FlashBlk_Init(T_ObjAddr obj, T_CodeAddr init);
extern U8 CalV_Init(T_ObjAddr obj, T_CodeAddr init);
extern U8 OutPin_Init(T_ObjAddr obj, T_CodeAddr init);
extern U8 Actuator_Init(T_ObjAddr obj, T_CodeAddr init);
extern U8 InPin_Init(T_ObjAddr obj, T_CodeAddr init);
extern U8 OutPort_Init(T_ObjAddr obj, T_CodeAddr init);
extern U8 Servo_UIInit(T_ObjAddr obj, T_CodeAddr init);
extern U8 NanoSM_Init(T_ObjAddr obj, T_CodeAddr init);

#ifdef _HAS_ACCS
   #if _HAS_ACCS == 1
   extern void Acc_Clear(T_ObjAddr obj);
   
   PRIVATE U8 Acc_Init(T_ObjAddr obj, T_CodeAddr init)
      { Acc_Clear( obj ); return 1; }
   #endif
#endif   


extern S16 Sensor_Read(T_ObjAddr obj);
extern S16 Scalar_Read(T_ObjAddr obj);
extern S16 OutPin_ReadInt(T_ObjAddr obj);
extern S16 Actuator_Read(T_ObjAddr obj);
extern S16 InPin_ReadInt(T_ObjAddr obj);
extern S16 OutPort_Read(T_ObjAddr obj);
extern S16 Acc_ReadAndClear(T_ObjAddr obj);

extern void Scalar_Write(T_ObjAddr obj, S16 n);
extern void OutPin_Write(T_ObjAddr obj, S16 n);
extern void Actuator_WrAlways(T_ObjAddr obj, S16 n);
extern void OutPort_Write(T_ObjAddr obj, S16 n);
extern void Acc_Add(T_ObjAddr obj, S16 n);

   #if USE_NANO_CAL == 1
extern U8 nCal_Init(T_ObjAddr obj, T_CodeAddr init);
extern S16 nCal_Read(T_ObjAddr obj);
extern void nCal_Write(T_ObjAddr obj, S16 n);
   #else
extern U8 Cal_Init(T_ObjAddr obj, T_CodeAddr init);
extern S16 Cal_Read(T_ObjAddr obj);
extern void Cal_Write(T_ObjAddr obj, S16 n);
   #endif

/*-----------------------------------------------------------------------------------------
|
|  Class_List
|
|  Your application's class table. 
|
|  The position of each entry must match its '_Class_xxxx' #def (in sysobj.h). If you are
|  not using a class then fill its slot with a null entry. Then it won't be linked in,
|  saving code space.
|
------------------------------------------------------------------------------------------*/

// If class names are not used then zero the strings

#ifndef USE_CLASS_NAMES
   #error "USE_CLASS_NAMES must be defined libs_support.h"
#else
   #if USE_CLASS_NAMES == 1
   #else
      #undef _StrLit
      #define _StrLit(n) 0
   #endif
#endif

// Must match the order of #define's _Class_xxxx

#define _sc    _ClassFlags_IsAScalar
#define _vec   _ClassFlags_IsAVector
#define _wr    _ClassFlags_IsWritable

PUBLIC CONST S_Class Class_List[] = 
{
   //    ID             Name              Init func      Read func         Write func        flags
   // --------------------------------------------------------------------------------------------------
   
         { 0,              0,                    0,             0,                0,                  0           },    // Null class 

   #ifndef _VECTOR_SUPPORT
      #error "_VECTOR_SUPPORT must be defined in bldcfg.h"
   #else
      #if _VECTOR_SUPPORT == 1
         { _Class_Vec,     _StrLit("Vector"),    Vec_Init,      0,                0,                  _vec | _wr  },  
      #endif
   #endif   

   #ifndef _HAS_SCALARS
      #error "_HAS_SCALARS must be defined in bldcfg.h"
   #else
      #if _HAS_SCALARS == 1
         { _Class_Scalar,  _StrLit("Scalar"),    Scalar_Init,   Scalar_Read,      Scalar_Write,       _sc | _wr   },    // 0 = no init function
      #endif
   #endif   

   #ifndef _HAS_LISTS
      #error "_HAS_LISTS must be defined in bldcfg.h"
   #else
      #if _HAS_LISTS == 1
         { _Class_List,    _StrLit("List"),      0,             0,                0,                  0           },
      #endif
   #endif   
   
   #ifdef _HAS_NANO_THREADS
         /* If this is defined AND the application threads are included in the master object list, Objs_List[],
            then the threads will be created by MakeObjsNano(). There's no need to make a separate list 
            of S_NanoSM_Cfg[] and pass it to NanoSM_InitList() in order to make the threads.
         */
         { _Class_Thread,  _StrLit("Thread"),    NanoSM_Init,             0,                0,                  0           },
   #endif

   #ifndef _HAS_THREADS
      #error "_HAS_THREADS must be defined in bldcfg.h"
   #else
      #if _HAS_THREADS == 1
         { _Class_Thread,  _StrLit("Thread"),    0,             0,                0,                  0           },
      #endif
   #endif   

   #ifndef _HAS_CALS
      #error "_HAS_CALS must be defined in bldcfg.h"
   #else
      #if _HAS_CALS == 1
         #if USE_NANO_CAL == 1
         { _Class_Cal,     _StrLit("Cal"),       nCal_Init,     nCal_Read,        nCal_Write,          _sc | _wr   },
         #else
         { _Class_Cal,     _StrLit("Cal"),       Cal_Init,      Cal_Read,         Cal_Write,          _sc | _wr   },
         #endif
      #endif
   #endif   

   #ifndef _HAS_NVSTORE
      #error "_HAS_NVSTORE must be defined in bldcfg.h"
   #else
      #if _HAS_NVSTORE == 1
         { _Class_NVStore, _StrLit("NVStore"),   FlashBlk_Init, 0,                0,                  0           },
      #endif
   #endif   

   #ifndef _HAS_CALVECS
      #error "_HAS_CALVECS must be defined in bldcfg.h"
   #else
      #if _HAS_CALVECS == 1
         { _Class_CalVec,  _StrLit("CalVector"),    CalV_Init,    0, 0, _vec | _wr },
      #endif
   #endif   

   #ifndef _HAS_OUTPINS
      #error "_HAS_OUTPINS must be defined in bldcfg.h"
   #else
      #if _HAS_OUTPINS == 1
         { _Class_OutPin,  _StrLit("OutPin"),    OutPin_Init,   OutPin_ReadInt,   OutPin_Write,       _sc | _wr   },
      #endif
   #endif   

   #ifndef _HAS_SENSORS
      #error "_HAS_SENSORS must be defined in bldcfg.h"
   #else
      #if _HAS_SENSORS == 1
         { _Class_Sensor,  _StrLit("Sensor"),    Sensor_Init,   Sensor_Read,      0,                  _sc         },
      #endif
   #endif   

   #ifndef _HAS_ACTUATORS
      #error "_HAS_ACTUATORS must be defined in bldcfg.h"
   #else
      #if _HAS_ACTUATORS == 1
         { _Class_Actuator,_StrLit("Actuator"),  Actuator_Init, Actuator_Read,    Actuator_WrAlways,  _sc | _wr   },
      #endif
   #endif   

   #ifndef _HAS_TIMERS
      #error "_HAS_TIMERS must be defined in bldcfg.h"
   #else
      #if _HAS_TIMERS == 1
         { _Class_Timer,   _StrLit("Timer"),     0,             0,                0,                  0           },
      #endif
   #endif   

   #ifndef _HAS_INPINS
      #error "_HAS_INPINS must be defined in bldcfg.h"
   #else
      #if _HAS_INPINS == 1
         { _Class_InPin,   _StrLit("InPin"),     InPin_Init,    InPin_ReadInt,    0,                  _sc         },
      #endif
   #endif   

   #ifndef _HAS_OUTPORTS
      #error "_HAS_OUTPORTS must be defined in bldcfg.h"
   #else
      #if _HAS_OUTPORTS == 1
         { _Class_OutPort, _StrLit("OutPort"),   OutPort_Init,  OutPort_Read,     OutPort_Write,    _sc | _wr   },
      #endif
   #endif   

   #ifndef _HAS_SERVO
      #error "_HAS_SERVO must be defined in bldcfg.h"
   #else
      #if _HAS_SERVO == 1
         { _Class_Servo, _StrLit("Servo"),       Servo_UIInit,  0,                0,                  0         },
      #endif
   #endif   

   #ifndef _HAS_ACCS
      #error "_HAS_ACCS must be defined in bldcfg.h"
   #else
      #if _HAS_ACCS == 1
         // Note: This class is from arith.h. Ensure that the prototypes line up.
         { _Class_Scalar,  _StrLit("ScalarAcc"),  Acc_Init,   Acc_ReadAndClear,      Acc_Add,       _sc | _wr   },    // 0 = no init function
      #endif
   #endif   


};

// Note: the Actuator write function is the one which bypasses the lock

#undef _sc
#undef _vec
#undef _wr

/*-----------------------------------------------------------------------------------------
|
|  NumClasses
|
------------------------------------------------------------------------------------------*/

PUBLIC U8 NumClasses(void) { return RECORDS_IN(Class_List); }



// ----------------------------------- eof ---------------------------------------


