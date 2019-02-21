#include <stdio.h>
#include <conio.h>
#include "lineedit.h"
#include "ui_test_support.h"

int main(void)
{
   LineEdit_Init();
   OStream_Reset();

   C8 ch = '\0';
   while( (ch = getch()) != 'q')
   {
      if( LineEdit_AddCh(ch) == 1)
      {
         LineEdit_AcceptLine();
      }
      //printf("> %c\r\n", ch);
   }
   printf("Tiny2 console build\r\n");
}
