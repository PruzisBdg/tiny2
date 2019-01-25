/*---------------------------------------------------------------------------
|
|                  Tiny1 - Trimknob-driven menuing
|
|  A small table-driven trimknob menuing system; up to 254 states.
|  Has support for backlight and powerdown/wakeup.
|
|  Used on Sequel Simulator
|
|
|
|
|
|
|
|      Rev 1.2   Jun 20 2011 12:00:52   spruzina
|   Exports TrimMenu_IsActive().
|
|      Rev 1.1   Feb 16 2010 13:40:32   spruzina
|   Add calls to show single items and lists on the trim menu. The screen is only written if the item being updated is actually displayed.
|
|      Rev 1.0   May 19 2009 12:56:32   spruzina
|   Initial revision.
|
|--------------------------------------------------------------------------*/

#ifndef TRIM_MENU_H
#define TRIM_MENU_H

#include "common.h"
#include "tiny_statem.h"
#include "matrix_orbital_lk_lcd.h"     // the character LCD module

// ================================= Menus ======================================

// Specifies one menu state

typedef struct
{
   // For any of the below, 0 means do/draw nothing

   LkLcd_S_DisplayList CONST     *showOnEnter;                 // Anything to draw/do upon entering this state
   void                          (*doOnTurn)(U8 clockwise);    // Upon turning the trimknob do this and,
   LkLcd_S_DisplayList CONST     *showOnTurn;                  // show the result
   U8                            goOnCW,                       // if != 0xFF then goto this state
                                 goOnCCW;
   void                          (*doOnPress)(void);           // Do upon pressing the button, do this and...
   LkLcd_S_ItemDisplaySpec CONST *toBlink;                     // Anything to be blinked?
   U8                            goOnPress,                    // then go here
                                 flags;
} TrimMenu_S_State;

// In the 'goOn..'  fields, this means stay at the current menu item
#define _Menu_StayPut 0xFF

// Bits in the 'flag'
#define _TrimMenu_ClearBlinkOnTurn     0
#define _TrimMenu_KeepBlinkOnTurn      _BitM(0)
#define _TrimMenu_ClearBlinkOnPress    0
#define _TrimMenu_KeepBlinkOnPress     _BitM(1)
#define _TrimMenu_IsAFullScreen        _BitM(2)
#define _TrimMenu_StartBlinkOn         _BitM(3)


// ================================== Backlight =======================================

PUBLIC U8 CONST *TrimMenu_GetBacklightModeStr(void);     // Returns name of teh current backlight mode; "dim", "bright" or "auto',
PUBLIC U8   TrimMenu_GetBacklightBright(void);           // The current 'brite' setting for the backlight
PUBLIC void TrimMenu_AdjBrightness(S8 incr);             // Adjust the 'brite' level
PUBLIC void TrimMenu_StepBacklightMode(U8 fwd);          // Goto next/previous backlight mode
PUBLIC void TrimMenu_SetToBright(void);                  // Set backlight to the 'brite' level
PUBLIC void TrimMenu_SetToDim(void);                     // Dim the backlight

PUBLIC void TrimMenu_ShowItem(LkLcd_S_ItemDisplaySpec CONST *n);     // Update display of 'n', (if it's currently visible)
PUBLIC void TrimMenu_ShowHdlList(LkLcd_S_DisplayHdlList CONST *h);   // Update display of items in 'h' (if currently visible)

// ==================================== Trimknob =========================================

typedef struct
{
   U8 clicksCW, clicksCCW;
} S_Trimknob;

// From the trimknob firmware
extern S_Trimknob IDATA Trimknob;
extern BIT TrimKnob_WasPressed;

#define TrimKnob_CW  1
#define TrimKnob_CCW 0

// Ballistic spin
PUBLIC S8 TrimMenu_GetSpin(void);
#define _TrimMenu_MaxSpin 5

// ====================== State Machine and Events to same ===============================

extern S_TinySM TrimMenu_StateM;
extern S_TinySM_Cfg CONST TrimMenu_StateM_Cfg;

#define _TrimMenu_Sleep TinySM_1stUserEvent           // From power control; to sleep the menus
#define _TrimMenu_Wake  (TinySM_1stUserEvent + 1)     // From power control; to wake the menus

PUBLIC BIT TrimMenu_IsActive(void);

// ===================== Your application must provide.... =================================

/* An array of menu states and a function returning the size of the array.

   This may be empty (NULL), (but then what's the point!)
*/
extern CONST TrimMenu_S_State AllTrimMenus[];
extern U8 TrimMenu_NumItems(void);

// An splash screen (which may NULL = empty )
extern CONST LkLcd_S_DisplayList SplashScreen;

// An application-specific init function (which may do nothing)
extern void TrimMenu_Init(void);


#endif // TRIM_MENU_H

// ------------------------------- eof --------------------------------------



