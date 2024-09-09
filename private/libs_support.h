/* -----------------------------------------------------------------------------------
|
| Support for (Renesas) ARM Tiny2 static and linked libraries.
|
| This file is specific to a project and has the same name in every project. Here it provides
| compile-time parameterisation for the Tiny2 library. The library calls will be 'weak'. If
| an application needs a different parameterization for a library function the app. should compile
| the necessary C-code directly and override the library version.
|
 -------------------------------------------------------------------------------------*/

#ifndef LIBS_SUPPORT_H
#define LIBS_SUPPORT_H

// ------------------------------------------ Tools ----------------------------------------------

#define TOOL_Z8_ENCORE 1
#define TOOL_RIDE_8051 2
#define TOOL_CC430     3
#define TOOL_GCC_ARM   4
#define TOOL_MPLAB_X32 5

#ifdef __TOOL_IS_GCC_ARM__
   #define _TOOL_IS TOOL_GCC_ARM
#else
   #ifdef __TOOL_IS_MPLAB_XC32__
      #define _TOOL_IS TOOL_MPLAB_X32
   #else
      #ifdef __TOOL_IS_CC430
         #define _TOOL_IS TOOL_CC430
      #endif
   #endif
#endif


#ifndef _TOOL_IS
   #error "_TOOL_IS must be defined"
#endif

#include "GenericTypeDefs.h"		// Redirect to typedefs

/* *************************** CONSTRUKSHUN ZONE ***************

    Right now the Tiny2_ lib is being built for Cortexm0+ only so build options below are
    are hardwired. Sort out Tools and Targets when we need more builds

****************************************************************/

// Global suspend/resume of interrupts.
#define _BlockAllInterrupts()
#define _UnblockEnabledInterrupts()

#define _SystemTick_msec 10.0      		// Renesas Synergy ThreadX default SysTick.

#define CODE_SPACE_20BIT 1
#define CODE_SPACE_IS CODE_SPACE_20BIT

#define DATA_SPACE_16BIT 1
#define DATA_SPACE_32BIT 2

   #if _TOOL_IS == TOOL_CC430
#define DATA_SPACE_IS DATA_SPACE_16BIT
   #else
#define DATA_SPACE_IS DATA_SPACE_32BIT
   #endif

#define BOOLEAN BOOL

#define _TARGET_8051_LIB_LGE_NANO   1
#define _TARGET_LIB_ARM             2
#define _TARGET_LIB_XC32            3

#define _TARGET_IS _TARGET_LIB_ARM

#define _HAS_TEXT_UI 1

#define _U8Ptr (U8*)

#define TicksPerSec 100

#define ResetTimer(timer) { *(timer) = _Now(); }

#define INCLUDE_HELP_TEXT

#define USE_NANO_STATEM 0

#define COMMSQ_SMALL 1
#define COMMSQ_LARGE 2
#define _COMMSQ_LEN_IS COMMSQ_LARGE

/* ----------------------------- Tiny1 printf() sprintf() Support --------------------------------------

   For those tools which do not have MCU-suitable printf support we use tiny1_ version. tiny1_stdio
   provides printf() and sprintf() but not scanf. The Tiny2 UI does use scanf() so we need a partial
   override of <stdio.h>
*/
#define _PRINTF_FROM_TINY1 1
#define _PRINTF_FROM_STDIO 2

#ifdef __USE_TINY1_PRINTF__
   #define _PRINTF_FROM  _PRINTF_FROM_TINY1
#else
   #define _PRINTF_FROM  _PRINTF_FROM_STDIO
#endif

#if _PRINTF_FROM == _PRINTF_FROM_TINY1 && _TOOL_IS == TOOL_GCC_ARM
   // Include floating point handling for all ARM targets (for now)
   #define TPRINT_IS_FLOAT
   #include "tiny1_stdio.h"

   /* Include <stdio.h> first. THEN rename printf() and sprintf() in all subsequent source.
    * Must rename because some tools e.g MPLabX don't support or use 'weak' and so their libs
    * can't be overridden.
   */
   #include <stdio.h>
   #define printf  tiny1_printf
   #define sprintf tiny1_sprintf
#else
   #include <stdio.h>
#endif

/* ------------------------ Linker controls ------------------------------

   For the library enable everything.
*/

#define USE_CLASS_NAMES 1

// User Interface
#define _HAS_TEXT_UI    1     // 1 = ANSI terminal at UART0; 0 = headless, no UART0
#define _HAS_TEXTFILES  1     // 1= textfile stotage in Flash, usually for scripting
#define _HAS_SCRIPTING  1     // 1 = runs scripts (from TextFiles)
#define _STORES_INTS    1     // 1 = files may be binary integer stores.

// Classes
#define _VECTOR_SUPPORT 1     // 'read', 'write' and 'compare' commands work on vectors
#define _HAS_ACCS       1     // Accumulator class

#define _HAS_SENSORS    1     // Sensor class
#define _HAS_OUTPINS    1     // OutPin class, controls single pin on/off
#define _HAS_OUTPORTS   1     // Array of OutPins
#define _HAS_INPINS     1     // InPins class, reads single pin, 1 or 0
#define _HAS_ACTUATORS  1     // Actuators class, analog outputs

#define _HAS_SCALARS    1
#define _HAS_CALS       1
#define _HAS_CALVECS    1

#define _HAS_THREADS    1     // Every app. has thread, but they need not be made as objects
#define _HAS_NVSTORE    1
#define _HAS_TIMERS     1     // Timer block, used by scripts etc
#define _HAS_LISTS      1     // Lists of objects
#define _HAS_SERVO      1     // Servo links Sensor and Actuator

// Tiny2_ has so many U8* to S8* pointer sign warning. Suppress these otherwise we're swamped.
#if _TOOL_IS == TOOL_CC430
#else
   #pragma GCC diagnostic ignored "-Wpointer-sign"
#endif



#endif // LIBS_SUPPORT_H

// ------------------------------- eof -----------------------------------------------

