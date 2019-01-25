/*---------------------------------------------------------------------------
|
|             Matrix-Orbital LKxxx Series Character LCD Modules
|
|  These have built-in character sets and RS232 or SMBus/I2C interface. They print
|  ASCII directly to the screen from the current cursor forward. Display control
|  sequences are escaped with 0xFE.
|
|  This file defines both the LCD control bytes and the interface to the drivers
|  which interface to the display.
|
|
|
|
|
|
|   
|      Rev 1.1   Feb 16 2010 13:41:48   spruzina
|   Add calls to test if a given display item is in a display list. Used by trim menues to find out if an item is currently visiable and shoudl be rewritten onscreen.
|   
|      Rev 1.0   May 19 2009 12:56:26   spruzina
|   Initial revision.
|  
|--------------------------------------------------------------------------*/

#ifndef MATRIX_ORBITAL_LK_LCD_H
#define MATRIX_ORBITAL_LK_LCD_H


// ======================= LKxxx LCD Interface =============================

/* This first isn't an LCD control char. It's inserted to the SMBus queue to 
   indicate the end of an LKxxx control sequence. The SMBus driver inserts a 
   delay before sending more queue content.
*/
#define _LCD_TypeLK_EndCtl    0xFF     

// This prefixes LCD control chars
#define _LCD_TypeLK_Ctl_Prefix   0xFE

// Control chars
#define _LCD_TypeLK_Ctl_I2CSlaveAddr           0x33
#define _LCD_TypeLK_Ctl_BaudRate               0x39
#define _LCD_TypeLK_Ctl_AutoScrollOn           0x51
#define _LCD_TypeLK_Ctl_AutoScrollOff          0x52
#define _LCD_TypeLK_Ctl_ClearScreen            0x58
#define _LCD_TypeLK_Ctl_SetStartupMsg          0x40
#define _LCD_TypeLK_Ctl_LineWrapOn             0x43
#define _LCD_TypeLK_Ctl_LineWrapOff            0x44
#define _LCD_TypeLK_Ctl_MoveTo                 0x47
#define _LCD_TypeLK_Ctl_CursorHome             0x48
#define _LCD_TypeLK_Ctl_CursorBack             0x4C
#define _LCD_TypeLK_Ctl_CursorFwd              0x4D
#define _LCD_TypeLK_Ctl_UnderlineCursorOn      0x4A
#define _LCD_TypeLK_Ctl_UnderlineCursorOff     0x4B
#define _LCD_TypeLK_Ctl_BlinkBlockCursorOn     0x53
#define _LCD_TypeLK_Ctl_BlinkBlockCursorOff    0x54
#define _LCD_TypeLK_Ctl_MakeCustomChar         0x4E
#define _LCD_TypeLK_Ctl_SaveCustomChar         0xC1
#define _LCD_TypeLK_Ctl_LoadCustomChar         0xC0
#define _LCD_TypeLK_Ctl_SaveStartupCustomChars 0xC2
#define _LCD_TypeLK_Ctl_InitMediumNumber       0x6D
#define _LCD_TypeLK_Ctl_PlaceMediumNumber      0x6F
#define _LCD_TypeLK_Ctl_InitLargeNumber        0x6E
#define _LCD_TypeLK_Ctl_PlaceLargeNumber       0x23
#define _LCD_TypeLK_Ctl_InitHorzBar            0x68
#define _LCD_TypeLK_Ctl_PlaceHorzBar           0x7C
#define _LCD_TypeLK_Ctl_InitNarrowVertBar      0x73
#define _LCD_TypeLK_Ctl_InitWideVertbar        0x76
#define _LCD_TypeLK_Ctl_PlaceVertBar           0x3D
#define _LCD_TypeLK_Ctl_DisplayOn              0x42
#define _LCD_TypeLK_Ctl_DisplayOff             0x46
#define _LCD_TypeLK_Ctl_SetBrightness          0x99
#define _LCD_TypeLK_Ctl_SetAndSaveBrightness   0x98
#define _LCD_TypeLK_Ctl_SetContrast            0x50
#define _LCD_TypeLK_Ctl_SetAndSaveContrast     0x91
#define _LCD_TypeLK_Ctl_ReadVersion            0x36
#define _LCD_TypeLK_Ctl_ReadType               0x37


// The default write slave-address
#define _SMBus_LCDWrSlaveAddr 0x50




/* ========================== Display Items and Lists ========================================
|
|  The display is controlled by lists of display items. Each item is something to be shown
|  on the display or a control to the display. 
|
|---------------------------------------------------------------------------------------------*/  

// Types of display items

typedef enum 
{
   E_LkLCDNone = 0,  // do nothing 
   E_Ctl,            // LKxxx display control byte,
   E_List,           // Points to an array of 'LkLcd_U_ItemToShow', of length 'flags.listSize'
   E_VarStr,         // RAM string
   E_ConstStr,       // ROM string
   E_ByteInline,     // a byte inline in the 'itemToShow' field
   E_Byte,           // a byte from the specified address
   E_Int,            // an int...
   E_Float,          // a float...
   E_GetByte,        // function returning a byte
   E_GetInt,         // ..     ..     ..     int
   E_GetFloat,       // ..     ..     ..     float
   E_GetStr,         // Copies out a string
   E_GetROMStr,      // function returning a CONST string ptr
   E_ObjID,          // the (byte) ID of a 'tiny1' object
   E_Erase           // just clear the field (of 'fieldWidth')
} LkLcd_E_DisplayObjType;


/* When instances of a union are initialised, the compiler will attempt to cast the 
   initialiser to the type of the first member. Use U16 as the 1st member, as all other
   members can be cast to it without being truncated.
*/
typedef union
{
   U16      asWord;                          // N.B This 1st member must be a word (see above). Used by...
                                             // E_Ctl, E_List, E_ObjID
   U8 CONST *constStr;                       // ROM strings (E_ConstStr)
   U8       *varStr;                         // RAM strings (E_VarStr)
   U8       *asBytePtr;                      // (E_Byte)
   S16      *asIntPtr;                       // (E_Int)
   float    *asFloatPtr;                     // (E_Float)
   void     *asVoidPtr;
   U8       (*getByte)(void);                // (E_GetByte)
   S16      (*getInt)(void);                 // (E_GetInt)
   float    (*getFloat)(void);               // (E_GetFloat)
   void     (*getStr)(U8 *buf, U8 maxBytes); // (E_GetStr)
   U8 CONST *(*getROMStr)(void);             // (E_GetROMStr)
} LkLcd_U_ItemToShow;


// What's in the 'flags' of 'S_ItemDisplaySpec'

typedef union
{
   U8 asFieldWidth,     // the width of the max field width for the item
      asListSize;       // the item is a list and this is it's length
} LkLcd_U_DisplaySpecFlags;


// The instructions to display 1 item

typedef struct
{
   U8                         rowOfs, colOfs;   // row, column offset relative to the current origin
   LkLcd_U_DisplaySpecFlags   flags;            // For an item to be displayed, is the field width
   LkLcd_E_DisplayObjType     itemType;         // what the item is, a variable, object, LCD control code etc.
   LkLcd_U_ItemToShow         itemToShow;       // the item itself
}  LkLcd_S_ItemDisplaySpec;

// and a list of display items

typedef struct
{
   LkLcd_S_ItemDisplaySpec CONST *items;        // the display items themselves
   U8    numItems,                              // how may in the list
         rowOfs, colOfs;                        // Origin for displaying this list
} LkLcd_S_DisplayList;


// and a list of handles to existing display items                                  
                                                                  
typedef struct
{
   LkLcd_S_ItemDisplaySpec CONST * CONST * items;  // display item addresses
   U8    numItems;                                 // how may in this list
} LkLcd_S_DisplayHdlList;

// Write an array of display items
PUBLIC void LkLcd_ShowItems(LkLcd_S_ItemDisplaySpec CONST *l, U8 listSize, U8 row0, U8 col0, U8 eraseAll);

PUBLIC void LkLcd_ShowList(LkLcd_S_DisplayList CONST *l);         // Show the contents of a display list
PUBLIC void LkLcd_ClearList(LkLcd_S_DisplayList CONST *l);        // Erase the contents of a display list

PUBLIC void LkLcd_ShowItem(LkLcd_S_ItemDisplaySpec CONST *l);     // Show one display item
PUBLIC void LkLcd_ClearItem(LkLcd_S_ItemDisplaySpec CONST *l);    // Clear one display item

PUBLIC void LkLcd_ShowHdlList(LkLcd_S_DisplayHdlList CONST *h);

PUBLIC BIT LkLcd_ListContainsItem(                                // Return 1 if 'n' is in 'l'
   LkLcd_S_DisplayList CONST *l, LkLcd_S_ItemDisplaySpec CONST *n);

PUBLIC BIT LkLcd_HaveItemsInCommon(                               // Return 1 if 'l1', l2' have any items in commmon
   LkLcd_S_DisplayList CONST *l1, LkLcd_S_DisplayHdlList CONST *l2);

PUBLIC void LkLcd_SetBrightness(U8 brightness);                   // Set display brightness
PUBLIC void LkLcd_SaveBrightness(U8 brightness);                  // Set brightness and have the display remember it.


#endif // MATRIX_ORBITAL_LK_LCD_H

// -------------------------------- eof -------------------------------------

 
