/*---------------------------------------------------------------------------
|
|           Driver for Matrix-Orbital LKxxx Series Character LCD Modules
|           with I2C interface.
|
|  The LKxxx LCD modules  have built-in character sets and RS232 or SMBus/I2C interface.
|  They print ASCII directly to the screen from the current cursor forward. Display
|  control sequences are escaped with 0xFE.
|
|  This code parses list of items to be displayed and queues them to be written to the
|  module.
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "tiny_statem.h"
#include "trim_menu.h"
#include "hostcomms.h"
#include <stdio.h>
#include "tty_ui.h"
#include "matrix_orbital_lk_lcd.h"
#include <string.h>
//#include "app_firm.h"
//#include "c8051_smbus.h"

//PUBLIC void Frm_SMBusInit(U8 slaveAddress, U8 clk);
//PUBLIC BIT Frm_SMBusWrBuf(U8 const GENERIC *buf, U8 cnt);
extern BIT Frm_SMBusWrStr(U8 const GENERIC *str);
extern BIT Frm_SMBusWrCh(U8 ch);
extern U8 Frm_SMBusBytesFree(void);


// An entry in the display context stack
typedef struct
{
   U8 row0, col0,                         // (row, col) origin for a display list
   listIdx,                               // The current list item
   listSize;                              // Number of items in the list
   LkLcd_S_ItemDisplaySpec CONST *head;   // List start
} S_DisplayContext;


// This stack allows lists within lists, up to '_DisplayStackSize' deep

#define _DisplayStackSize 4

typedef struct
{
   S_DisplayContext  buf[_DisplayStackSize];
   U8                top;                       // the empty top, ready for the next push.
} S_ContextStack;

PRIVATE S_ContextStack cs;


/*-----------------------------------------------------------------------------------------
|
|  moveTo()
|
|  Send LCD control code to move cursor to (row,col). Note: top-left is (1,1); BASIC-style
|  indexing. Before writing make sure there's room for the whole sequemce in the send
|  buffer. This is important as the display gets garbled if control sequences are sliced.
|
|  Note; LKxxx syntax is <MoveTo> <col> <row> (col is 1st!)
|
--------------------------------------------------------------------------------------------*/

PRIVATE void moveTo(U8 row, U8 col)
{
   if( Frm_SMBusBytesFree() > 5 )
   {
      Frm_SMBusWrCh(_LCD_TypeLK_Ctl_Prefix);
      Frm_SMBusWrCh(_LCD_TypeLK_Ctl_MoveTo);
      Frm_SMBusWrCh(col);
      Frm_SMBusWrCh(row);
      Frm_SMBusWrCh(_LCD_TypeLK_EndCtl);        // and append end-of-control flag to force a delay in SMBus traffic
   }
}

/*-----------------------------------------------------------------------------------------
|
|  wrCtlParm()
|
|  Write a control + a single byte parameter to the LCD module. First make sure there's
|  room for the whole sequemce in the send buffer. This is important as the display gets
|  garbled if control sequences are sliced.
|
--------------------------------------------------------------------------------------------*/

PRIVATE void wrCtlParm(U8 ctl, U8 parm)
{
   if( Frm_SMBusBytesFree() > 4 )
   {
      Frm_SMBusWrCh(_LCD_TypeLK_Ctl_Prefix);
      Frm_SMBusWrCh(ctl);
      Frm_SMBusWrCh(parm);
      Frm_SMBusWrCh(_LCD_TypeLK_EndCtl);        // and append end-of-control flag to force a delay in SMBus traffic
   }
}

/*-----------------------------------------------------------------------------------------
|
|  LkLcd_SetBrightness()
|
--------------------------------------------------------------------------------------------*/

PUBLIC void LkLcd_SetBrightness(U8 brightness)
{
   wrCtlParm(_LCD_TypeLK_Ctl_SetBrightness, brightness);
}

/*-----------------------------------------------------------------------------------------
|
|  LkLcd_SaveBrightness()
|
--------------------------------------------------------------------------------------------*/

PUBLIC void LkLcd_SaveBrightness(U8 brightness)
{
   wrCtlParm(_LCD_TypeLK_Ctl_SetAndSaveBrightness, brightness);
}

#define _Show  0
#define _Erase 1

/*-----------------------------------------------------------------------------------------
|
|  LkLcd_ShowList()
|
|  Write display list 'l' to the LCD
|
--------------------------------------------------------------------------------------------*/

PUBLIC void LkLcd_ShowList(LkLcd_S_DisplayList CONST *l)
{
   LkLcd_ShowItems(l->items, l->numItems, l->rowOfs, l->colOfs, _Show);
}

/*-----------------------------------------------------------------------------------------
|
|  LkLcd_ShowHdlList()
|
|  Write display handle list 'h' to the LCD
|
--------------------------------------------------------------------------------------------*/

PUBLIC void LkLcd_ShowHdlList(LkLcd_S_DisplayHdlList CONST *h)
{
   U8 c;

   for( c = 0; c < h->numItems; c++ )
      { LkLcd_ShowItem(h->items[c]); }
}

/*-----------------------------------------------------------------------------------------
|
|  LkLcd_ClearList()
|
|  On the LCD, blank all fields in display list 'l'
|
--------------------------------------------------------------------------------------------*/

PUBLIC void LkLcd_ClearList(LkLcd_S_DisplayList CONST *l)
{
   LkLcd_ShowItems(l->items, l->numItems, l->rowOfs, l->colOfs, _Erase);
}

/*-----------------------------------------------------------------------------------------
|
|  LkLcd_ShowItem()
|
|  On the LCD, show a single item 'l' relative to (0,0)
|
--------------------------------------------------------------------------------------------*/

PUBLIC void LkLcd_ShowItem(LkLcd_S_ItemDisplaySpec CONST *l)
{
   LkLcd_ShowItems(l, 1, 0, 0, _Show);
}

/*-----------------------------------------------------------------------------------------
|
|  LkLcd_ClearItem()
|
|  On the LCD, erase a single item 'l' relative to (0,0)
|
--------------------------------------------------------------------------------------------*/

PUBLIC void LkLcd_ClearItem(LkLcd_S_ItemDisplaySpec CONST *l)
{
   LkLcd_ShowItems(l, 1, 0, 0, _Erase);
}

/*-----------------------------------------------------------------------------------------
|
|  LkLcd_DisplayItems()
|
|  Display the list of items 'l[]' of length 'listSize', relative to (row0, col0)
|
--------------------------------------------------------------------------------------------*/

PUBLIC void LkLcd_ShowItems(LkLcd_S_ItemDisplaySpec CONST *l, U8 listSize, U8 row0, U8 col0, U8 eraseAll)
{
   #define _FormatBufSize 21        // The width of the display + 1

   U8    c,
         row, col,                  // Coord for the current display item
         itemType;                  // and type of the item
   char  buf[_FormatBufSize];       // Buffer for formatting

   S_DisplayContext dc;                   // current list head, index etc
   LkLcd_S_ItemDisplaySpec CONST *ds;     // entire spec for the item to be displayed
   LkLcd_U_ItemToShow   di;               // the actual item inside the spec

   cs.top = 0;                                        // Initialise context stack pointer to top

   dc.head = l;
   dc.listSize = listSize;                            // Initialise the current context from the parms passed
   dc.row0 = row0;
   dc.col0 = col0;
   dc.listIdx = 0;                                    // and we are at the start of the list

   while(1)                                           // For each item (to display) in the list
   {
      ds = &dc.head[dc.listIdx];                      // Ptr to the complete display spec for one item
      di = ds->itemToShow;                            // Ptr to the actual item to display
      itemType = ds->itemType;                        // and what it is.

      /* Check for end-of-list here, before processing any list items. This is necessary
         to work with sub-lists (type 'E_List'). After pushing into a sub-list, the loop
         returns to here with the 'listIdx' initilaised to 0. Because of this, if this check
         is done after switch(itemType) then the last item in the sublist will be skipped.
      */
      if( dc.listIdx++ >= dc.listSize )               // At end of list?
      {
         if(!cs.top)                                  // Top of list stack
         {
            break;                                    // then we're done; break out of the display loop
         }
         else                                         // else not at top of list stack
         {
            cs.top--;                                 // so pop the stack
            dc = cs.buf[cs.top];                      // and retrieve the current context
            continue;                                 // and continue in the display loop
         }
      }

      row = dc.row0 + ds->rowOfs;                     // Location relative to (row0, col0)
      col = dc.col0 + ds->colOfs;

      switch(itemType)
      {
         case E_LkLCDNone:                            // The NULL item?
            break;                                    // do nothing.. goto next one

         case E_Ctl:                                  // Send a 1-byte control code the the display
            if( Frm_SMBusBytesFree() > 3 )            // The queue has room for for the whole 3-byte sequence?
            {
               Frm_SMBusWrCh(_LCD_TypeLK_Ctl_Prefix); // then write Prefix
               Frm_SMBusWrCh(di.asWord);              // then the control itself
               Frm_SMBusWrCh(_LCD_TypeLK_EndCtl);     // and append end-of-control flag to force a delay in SMBus traffic
            }
            break;

         case E_List:                                 // The head of another list
            if( cs.top < _DisplayStackSize-1 )        // We can drop into this list?
            {
               cs.buf[cs.top] = dc;                   // So copy current context onto stack
               cs.top++;                              // and push it!

               dc.head = (LkLcd_S_ItemDisplaySpec CONST *)di.asVoidPtr;  // Set the new list head
               dc.listSize = ds->flags.asListSize;
               dc.listIdx = 0;                        // and initialise new list pointer to the list start
               dc.row0 = row;                         // List contents will be displayed relative to coords of the list head
               dc.col0 = col;
            }
            break;

         default:                                     // The rest have (x,y) coords

            moveTo(row,col);

            for(c = ds->flags.asFieldWidth; c; c--)
               { Frm_SMBusWrCh(' '); }

            moveTo(row,col);

            if(itemType == E_Erase || eraseAll)       // Just clear the field?
            {
               break;                                 // was already cleared (above), so done
            }
            else                                      // else there's something to write in the cleared space
            {
               switch(itemType)
               {
                  case E_ByteInline:
                     Frm_SMBusWrCh(di.asWord);        // a byte?
                     break;

                  case E_ConstStr:
                     Frm_SMBusWrStr(di.constStr);     // A constant string?
                     break;

                  case E_VarStr:                      // A non-constant string?
                     Frm_SMBusWrStr(di.varStr);
                     break;

                  default:                            // Remainder of items are formatted (into 'buf[]')
                     switch(itemType)
                     {
                        case E_Byte:
                           sprintf(buf, "%d", *(di.asBytePtr));
                           break;

                        case E_Int:
                           sprintf(buf, "%d", *(di.asIntPtr));
                           break;

                        case E_Float:
                           sprintf(buf, "%4.3f", *(di.asFloatPtr));
                           break;

                        case E_GetByte:
                           sprintf(buf, "%d", (S16)(di.getByte)() );
                           break;

                        case E_GetInt:
                           sprintf(buf, "%d", (di.getInt)() );
                           break;

                        case E_GetFloat:
                           sprintf(buf, "%4.3f", (di.getFloat)() );
                           break;

                        case E_GetStr:
                           di.getStr((U8*)buf, ds->flags.asFieldWidth);
                           break;

                        case E_GetROMStr:
                           strcpy(buf, (char const*)(di.getROMStr)() );
                           break;

                        case E_ObjID:
                           UI_SPrintScalarObject(buf, GetObjByIdx(di.asWord), _UI_PrintObject_AppendUnits);
                           break;

                     }
                     buf[ds->flags.asFieldWidth] = '\0';         // Make sure field is not overwritten
                     Frm_SMBusWrStr((U8 const*)buf);
               }
            }
      }     // end: Item display
   }  // end: while(1)
}


/*-----------------------------------------------------------------------------------------
|
|  LkLcd_ListContainsItem()
|
|  Return 1 if display item 'n' is in display list 'l'.
|
--------------------------------------------------------------------------------------------*/

PUBLIC BIT LkLcd_ListContainsItem(LkLcd_S_DisplayList CONST *l, LkLcd_S_ItemDisplaySpec CONST *n)
{
   U8 c;

   for(c = 0; c < l->numItems; c++)
   {
      if( &l->items[c] == n )
         { return 1; }
   }
   return 0;
}

/*-----------------------------------------------------------------------------------------
|
|  LkLcd_HaveItemsInCommon()
|
|  Return 1 if display lists 'l1', 'l2' have any items in common.
|
--------------------------------------------------------------------------------------------*/

PUBLIC BIT LkLcd_HaveItemsInCommon(LkLcd_S_DisplayList CONST *l1, LkLcd_S_DisplayHdlList CONST *l2)
{
   U8 c;

   for(c = 0; c < l2->numItems; c++)
   {
      if( LkLcd_ListContainsItem(l1, l2->items[c]) )
         { return 1; }
   }
   return 0;
}

// ------------------------------- eof --------------------------------------



