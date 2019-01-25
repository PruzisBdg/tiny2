/*---------------------------------------------------------------------------
|
|            Test Fixtures Support - System Classes
|
|--------------------------------------------------------------------------*/

#ifndef CLASS_H
#define CLASS_H

#include "common.h"
#include "sysobj.h"

#define _Class_Null     0
#define _Class_Vec      1 
#define _Class_Scalar   2
#define _Class_List     3    // List of objects
#define _Class_Thread   4    // Threads / State machines
#define _Class_Cal      5    // Calibrations
#define _Class_NVStore  6
#define _Class_CalVec   7    // Calibration vectors
#define _Class_OutPin   8    // Output pins
#define _Class_Sensor   9
#define _Class_Actuator 10
#define _Class_Timer    11
#define _Class_InPin    12
#define _Class_OutPort  13
#define _Class_Servo    14
#define _Class_Acc      15    // Scalar accumulator, cleared on read.

// Class specification

typedef struct
{
   U8          id;                                    // unique ID (above)
   C8 CONST    *name;                                 // used by the UI
   U8(*initFunc)(T_ObjAddr obj, T_CodeAddr init);     // constructor
   S16 (*readFunc)(T_ObjAddr obj);                    // most classes can be read
   void (*wrFunc)(T_ObjAddr obj, S16 n);              // many classes can be written
   U8          flags;
} S_Class;

#define _ClassFlags_IsAScalar    _BitM(0)
#define _ClassFlags_IsAVector    _BitM(1)
#define _ClassFlags_IsWritable   _BitM(2)

// To query class data
PUBLIC U8 GetClassFlags( S_Obj CONST * obj );
PUBLIC U8 Obj_IsAScalar( S_Obj CONST * obj);
PUBLIC U8 Obj_IsWritable( S_Obj CONST * obj);
PUBLIC U8 Obj_IsWritableScalar( S_Obj CONST * obj);
PUBLIC U8 Obj_IsAVector( S_Obj CONST * obj);
PUBLIC U8 Obj_IsWritableVector( S_Obj CONST * obj);

PUBLIC U8 CONST * GetClassName(U8 classID);


/* Every application has this list of classes which it uses

   Unused classes are not linked in to save code space.
*/
PUBLIC S_Class CONST * GetClass(U8 classID);


#endif

// ------------------------------------ eof -------------------------------------

 
