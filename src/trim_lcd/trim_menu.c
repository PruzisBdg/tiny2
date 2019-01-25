/*---------------------------------------------------------------------------
|
|                       Trimknob-driven menuing
|
|  A small table-driven trimknob menuing system on a character LCD; up to
|  254 states.
|
|  Has support for backlight and powerdown/wakeup.
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "tiny_statem.h"
//#include "app_firm.h"
#include "matrix_orbital_lk_lcd.h"
#include "trim_menu.h"
#include "systime.h"
#include "arith.h"
//#include "c8051_smbus.h"


// Interface to the trimknob firmware
PUBLIC S_Trimknob IDATA Trimknob;         // clockwise and CCW counts
PUBLIC BIT TrimKnob_WasPressed;


// To navigate the menus.
typedef struct
{
   TrimMenu_S_State CONST *item;             // Current menu state
   TrimMenu_S_State CONST *lastFullScreen;   // Most recent whole screen, for recovery after power-down
   T_Timer                t1;
   S8                     spin;
} S_TrimMenu;

PRIVATE S_TrimMenu tm;

PRIVATE BIT blink = 0;     // For blinking fields
PRIVATE U8 splashTimer;    // Times the splash screen
PRIVATE BIT wasCW;         // direction of the previous trimknob turn


// ================================= Backlight Control =============================================

// The 3 backlight modes
typedef enum
{
   E_AlwaysOn = 0,
   E_AlwaysOff,
   E_DimWhenUnused
} T_BacklightMode;

// To control the backlight
typedef struct
{
   T_BacklightMode   mode;                // Current backlight mode
   U8                hiBrightness;        // Current 'brite' setting
   T_Timer           mostRecentActivity;  // Most recent trimknob turn, LED drive or incoming serial port data
} S_Backlight;

PRIVATE S_Backlight backlight;



/*-----------------------------------------------------------------------------------------
|
|  TrimMenu_StepBacklightMode()
|
|  Rotate around the 3 backlight modes; alwaysOn, alwaysOff and dimWhenUnused.
|
------------------------------------------------------------------------------------------*/

PUBLIC void TrimMenu_StepBacklightMode(U8 fwd)
{
   backlight.mode =
   (fwd)
   ?
      ((backlight.mode >= E_DimWhenUnused) ? E_AlwaysOn : backlight.mode+1)
   :
      ((backlight.mode == E_AlwaysOn) ? E_DimWhenUnused : backlight.mode-1);

   // Update the backlight from the new mode
   if( backlight.mode == E_AlwaysOff )          // Always off?
   {
      LkLcd_SetBrightness(0);                   // then kill it now
   }
   else
   {
      TrimMenu_SetToBright();                   // otherwise, just changed modes so start at brite
   }
}


/*-----------------------------------------------------------------------------------------
|
|  TrimMenu_GetBacklightMode()
|
|  Return a string naming the current backlight mode.
|
------------------------------------------------------------------------------------------*/

PRIVATE U8 CONST str_On[] = "on";
PRIVATE U8 CONST str_Off[] = "off";
PRIVATE U8 CONST str_Auto[] = "auto";

PUBLIC U8 CONST *TrimMenu_GetBacklightModeStr(void)
{
   switch( backlight.mode )
   {
      case E_AlwaysOn:        return str_On;
      case E_AlwaysOff:       return str_Off;
      case E_DimWhenUnused:   return str_Auto;
      default:                return str_Auto;
   }
}

/*-----------------------------------------------------------------------------------------
|
|  TrimMenu_SetToBright()
|
|  Apply the current 'bright' level
|
------------------------------------------------------------------------------------------*/

PUBLIC void TrimMenu_SetToBright(void)
{
   LkLcd_SaveBrightness(backlight.hiBrightness);
}


/*-----------------------------------------------------------------------------------------
|
|  TrimMenu_SetToDim()
|
|  Apply the current 'dim' level
|
------------------------------------------------------------------------------------------*/

PUBLIC void TrimMenu_SetToDim(void)
{
   // Dim is 1/3 of bright, but not less than 10
   LkLcd_SetBrightness(MaxU8(backlight.hiBrightness/4, 10));
}


/*-----------------------------------------------------------------------------------------
|
|  TrimMenu_GetBacklightBright()
|
|  Return the current 'bright' level
|
------------------------------------------------------------------------------------------*/

PUBLIC U8 TrimMenu_GetBacklightBright(void) {return backlight.hiBrightness;}


/*-----------------------------------------------------------------------------------------
|
|  TrimMenu_AdjBrightness()
|
|  Step the current bright level by 'incr', which may positive (to brighten) or
|  negative (to dim).
|
------------------------------------------------------------------------------------------*/

PUBLIC void TrimMenu_AdjBrightness(S8 incr)
{
   backlight.hiBrightness = ClipS16toU8((S16)backlight.hiBrightness + incr);
   TrimMenu_SetToBright();
}

// ======================================= end: Backlight ======================================


/*-----------------------------------------------------------------------------------------
|
|  TrimMenu_ShowItem()
|
|  Update display item 'n', if it's on the current screen.
|
------------------------------------------------------------------------------------------*/

PUBLIC void TrimMenu_ShowItem(LkLcd_S_ItemDisplaySpec CONST *n)
{
   if( LkLcd_ListContainsItem(tm.lastFullScreen->showOnEnter, n) )
   {
      LkLcd_ShowItem(n);
   }
}


/*-----------------------------------------------------------------------------------------
|
|  TrimMenu_ShowHdlList()
|
|  Update the display of items in handle list 'h', if any are on the current screen.
|
------------------------------------------------------------------------------------------*/

PUBLIC void TrimMenu_ShowHdlList(LkLcd_S_DisplayHdlList CONST *h)
{
   if( LkLcd_HaveItemsInCommon(tm.lastFullScreen->showOnEnter, h) )
   {
      LkLcd_ShowHdlList(h);
   }
}




/*-----------------------------------------------------------------------------------------
|
|  TrimMenu_GetSpin()
|
|  Return a ballistic 'spin' count from the trimknob, in the range 1 - 5 (clockwise) or
|  -1 to -5 (counter-clockwise)
|
------------------------------------------------------------------------------------------*/

PUBLIC S8 TrimMenu_GetSpin(void)
{
   return tm.spin;
}




/*-----------------------------------------------------------------------------------------
|
|  doOnEnter()
|
|  To do upon entering a new menu state.
|
------------------------------------------------------------------------------------------*/

PRIVATE void doOnEnter(void)
{
   if(tm.item->showOnEnter)                                 // Things to display?
      { LkLcd_ShowList(tm.item->showOnEnter); }             // then show them

   if( BSET(tm.item->flags, _TrimMenu_IsAFullScreen) )      // This menu item is a full screen?
   {
      tm.lastFullScreen = tm.item;                          // then mark this as the current full screen
   }                                                        // (for screen refresh after 'sleep' )

   if(tm.item->toBlink)                                     // Something to blink?
   {
      if( BSET(tm.item->flags, _TrimMenu_StartBlinkOn) )    // That something is to show rightway?
      {
         LkLcd_ShowItem(tm.item->toBlink);                  // then start with it on
         blink = 1;
      }
      else
      {
         LkLcd_ClearItem(tm.item->toBlink);                 // else start with it off
         blink = 0;
      }
   }
}

/*-----------------------------------------------------------------------------------------
|
|  doOnExit()
|
|  To do upon leaving the current menu state.
|
------------------------------------------------------------------------------------------*/

PRIVATE void doOnExit(U8 blinkFlag)
{
   if(tm.item->toBlink)                                  // Something was blinking?
   {
      if( BSET(tm.item->flags, blinkFlag) )              // instructed to leave it up?
      {
         LkLcd_ShowItem(tm.item->toBlink);               // then make sure it's on
      }
      else
      {
         LkLcd_ClearItem(tm.item->toBlink);              // else make sure it's off.
      }
      blink = 0;
   }
}

// ================================= State Machine =======================================

#define _sStartup       1        // Time for the LCD module to boot up
#define _sSplashScreen  2        // Show the splash screen
#define _sRunMenu       3        // Run the menus
#define _sSleep         4        // Sleep the LCD module
#define _sWakeup        5        // LCD is waking up

#define _BlinkIntvl_secs 0.35

/*-----------------------------------------------------------------------------------------
|
|  initMenu()
|
------------------------------------------------------------------------------------------*/

PRIVATE U8 initMenu(void)
{
   Trimknob.clicksCW = 0;
   Trimknob.clicksCCW = 0;
   TrimKnob_WasPressed = 0;
   TrimMenu_Init();
   splashTimer = 2.0 / _BlinkIntvl_secs;     // Wait 2 secs before showing splash screen
   return 1;
}

/*-----------------------------------------------------------------------------------------
|
|  startup()
|
|  Write the splash screen. Leave it up for 5secs
|
------------------------------------------------------------------------------------------*/

PRIVATE U8 startup(S_TinySM_EventList *events)
{
   if( !--splashTimer )                         // LCD's own starup and splash done?
   {
      LkLcd_ShowList(&SplashScreen);            // Show the splash screen
      splashTimer = 5.0 / _BlinkIntvl_secs;     // for the next 5 secs.
      return _sSplashScreen;
   }
   else                                         // else LK204 LCD still booting.
   {
      return _sStartup;
   }
}

/*-----------------------------------------------------------------------------------------
|
|  splashScreen()
|
|  Displaying the splash sceeen. When splash done repalce with the entry menu.
|
------------------------------------------------------------------------------------------*/

PRIVATE U8 splashScreen(S_TinySM_EventList *events)
{
   if( !--splashTimer )                               // Splash display period done?
   {
      LkLcd_ShowList(AllTrimMenus->showOnEnter);      // Show the startup screen
      tm.item  = &AllTrimMenus[0];                    // Start at top of list (Home)
      tm.lastFullScreen = tm.item;                    // Presume that the 1st item in the menu is a full screen

      backlight.mode = E_DimWhenUnused;
      backlight.hiBrightness = 150;
      ResetTimer(&backlight.mostRecentActivity);
      TrimKnob_WasPressed = 0;                        // Trimknob was pressed to power-on. Clear this now.
      tm.spin = 1;                                    // Init ballistic spin counte to a legal value

      return _sRunMenu;                               // and go to processing any user input
   }
   else
      { return _sSplashScreen; }
}

/*-----------------------------------------------------------------------------------------
|
|  runMenu()
|
|  Run the menu in response to trimknob turns and presses. If told to sleep then do so.
|
------------------------------------------------------------------------------------------*/

PRIVATE U8 runMenu(S_TinySM_EventList *events)
{
   BIT turned, pressed;

   if( TinySM_GetEvent(events, _TrimMenu_Sleep) )        // MCU will sleep. LCD is off?
   {
      return _sSleep;                                    // Go here until woken.
   }

   if( TinySM_GetEvent(events, TinySM_Event_Trig1) )     // Trimknob turn or button-press
   {
      turned = 0;
      pressed = 0;

      if(Trimknob.clicksCW)                              // Was turned CW
      {
         /* Update the ballistic 'spin' counter. Note that the counter is updated before
            doing the turn method. If the knob has not been turned for a while then 'spin'
            will have been decremented to 0 and so must be incremented to +/-1 before being
            applied to the turn method. Also, if the knob has reversed then this resets the
            ballistic 'spin' so there's no overrun.
         */
         if(wasCW)                                       // Another clockwise click?
         {
            if(tm.spin < _TrimMenu_MaxSpin)
               { tm.spin++; }                            // then increment the ballistic counter, (5) max
         }
         else                                            // else previous click was CCW
         {
            wasCW = 1;                                   // Mark new direction.
            tm.spin = 1;                                 // and start ballistic counter at 1
         }

         // Do the 'turn' method, if any

         if(tm.item->doOnTurn)                           // There's a CW turn method?
         {
            tm.item->doOnTurn(TrimKnob_CW);              // then execute this method
         }
         Trimknob.clicksCW = 0;                          // and reset the CW count

         if( tm.item->goOnCW < TrimMenu_NumItems() )     // On-turn destination is inside state-list?
         {
            doOnExit(_TrimMenu_KeepBlinkOnTurn);         // then clean up and...
            tm.item = &AllTrimMenus[tm.item->goOnCW];    // go to the specified destination
            doOnEnter();                                 // Init screen for new menu state
         }

         turned = 1;                                     // Mark that we turned, for below
      }

      if(Trimknob.clicksCCW)                             // Was turned CCW?
      {
         /* Update the ballistic 'spin' counter. Note that the counter is updated before
            doing the turn method. If the knob has not been turned for a while then 'spin'
            will have been decremented to 0 and so must be incremented to +/-1 before being
            applied to the turn method. Also, if the knob has reversed then this resets the
            ballistic 'spin' so there's no overrun.
         */
         if( !wasCW )                                    // Another counter-clockwise click
         {
            if(tm.spin > -_TrimMenu_MaxSpin)
               { tm.spin--; }                            // then increment the ballistic counter towards (-5)
         }
         else                                            // else previous click was CW
         {
            wasCW = 0;                                   // Mark the new direction
            tm.spin = -1;                                // and start ballistic counter at -1
         }

         // Do the 'turn' method, if any

         if(tm.item->doOnTurn)                           // etc...
         {
            tm.item->doOnTurn(TrimKnob_CCW);
         }
         Trimknob.clicksCCW = 0;


         if( tm.item->goOnCCW < TrimMenu_NumItems() )    // On-turn destination is outside state-list?
         {
            doOnExit(_TrimMenu_KeepBlinkOnTurn);         // then clean up and...
            tm.item = &AllTrimMenus[tm.item->goOnCCW];   // go to the specified destination
            doOnEnter();                                 // Init screen for new menu state
         }

         turned = 1;                                     // Mark that knob turned, for below
      }

      if(turned)                                         // Trimknob was turned either way?
      {
         if(tm.item->showOnTurn)                         // and there's something to update on the display?
         {
            LkLcd_ShowList(tm.item->showOnTurn);         // then show it.
         }

      }

      if( TrimKnob_WasPressed )                          // Trimknob was pressed?
      {
         TrimKnob_WasPressed = 0;                        // then clear this flag
         pressed = 1;

         if(tm.item->doOnPress)                          // Is there an on-press method
         {
            tm.item->doOnPress();                        // then do that method
         }
         if( tm.item->goOnPress < TrimMenu_NumItems() )  // On-press destination is outside state-list?
         {
            doOnExit(_TrimMenu_KeepBlinkOnPress);        // then clean up and..
            tm.item = &AllTrimMenus[tm.item->goOnPress]; // go to the specified destination
            doOnEnter();                                 // Init screen for new menu state
         }
      }

      if( turned || pressed )                            // Trimknob was moved?
      {
         ResetTimer(&backlight.mostRecentActivity);      // then mark the time

         if(backlight.mode != E_AlwaysOff)               // Backlight mode isn'r 'off'?
            { TrimMenu_SetToBright(); }                  // then brite the display now
      }
   }

   // Every 0.4 secs......

   if( TinySM_GetEvent(events, TinySM_Event_Tick) )
   {
      if( ElapsedS(backlight.mostRecentActivity) > _TicksSec_S(20) )     // More than 20secs since trimknob moved?
      {
         if(backlight.mode == E_DimWhenUnused)           // Backlight mode is auto-dim?
            { TrimMenu_SetToDim(); }                     // then dim backlight now
      }

      // Decrement the ballistic spin counter towards 0. On the next knob turn it will
      // be preincremented towards 1 or -1 before being applied to the knob 'turn' method.

      if( tm.spin > 0)                                   // > 0 => clockwise?
      {
         tm.spin--;                                      // then still clockwise but less so
      }
      else if( tm.spin < 0 )                             // < 0 => counter-clockwise
      {
         tm.spin++;                                      // then still ccw but less so
      }

      if(tm.item->toBlink)                               // There's something to blink?
      {
         if(blink)                                       // On?
            { LkLcd_ShowItem(tm.item->toBlink); }        // then show that something
         else
            { LkLcd_ClearItem(tm.item->toBlink); }       // else hide that something
         blink = !blink;                                 // and toggle the blink flag
      }


   }
   return _sRunMenu;       // Return this state
}


/*-----------------------------------------------------------------------------------------
|
|  sleep()
|
|  Display is off. Wait for wakeup
|
------------------------------------------------------------------------------------------*/

PRIVATE U8 sleep(S_TinySM_EventList *events)
{
   if( TinySM_GetEvent(events, _TrimMenu_Wake) )
   {
      ResetTimer(&tm.t1);
      return _sWakeup;
   }
   else
   {
      return _sSleep;
   }
}


/*-----------------------------------------------------------------------------------------
|
|  wakeup()
|
|  Power to display was reenabled. Allow a couple of secs for display module to boot and
|  then send it the current screen.
|
------------------------------------------------------------------------------------------*/

PRIVATE U8 wakeup(S_TinySM_EventList *events)
{
   if( ElapsedS(tm.t1) > _TicksSec_S(2) )                    // Wait 2 secs for disply to init
   {
      if( tm.lastFullScreen->showOnEnter )                  // There was a screen when display powered down?
      {
         LkLcd_ShowList(tm.lastFullScreen->showOnEnter);    // then resend that screen now
         TrimKnob_WasPressed = 0;                           // If knob was pressed to wake the display,
                                                            // we don't want to process that press. So clear it now.
      }
      return _sRunMenu;                                     // and back to processing trimknob activity.
   }
   else
   {
      return _sWakeup;
   }
}


// ----------------- Menu Handling State Machine --------------------------------

PRIVATE U8(* const stateFuncs[])(S_TinySM_EventList*) =
{
   0,             // no entry function
   startup,
   splashScreen,   // Show the startup screen
   runMenu,       // Respond to user input
   sleep,
   wakeup
};


PUBLIC S_TinySM TrimMenu_StateM;

PUBLIC S_TinySM_Cfg CONST TrimMenu_StateM_Cfg =
{
   RECORDS_IN(stateFuncs)-1,        // 2 states (excluding entry function)
   0,                               // no flags - default behaviour
   _TicksSec_S(_BlinkIntvl_secs),    // For blinking items
   stateFuncs,                      // list of state functions
   initMenu                         // state machine init
};


/*-----------------------------------------------------------------------------------------
|
|  TrimMenu_IsActive()
|
------------------------------------------------------------------------------------------*/

PUBLIC BIT TrimMenu_IsActive(void)
{
   return TrimMenu_StateM.state == _sRunMenu;
}

// ------------------------------- eof --------------------------------------



