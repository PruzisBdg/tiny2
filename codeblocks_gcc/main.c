#include <stdio.h>
#include <conio.h>
#include "lineedit.h"

int main(void)
{
   LineEdit_Init();

   C8 ch = '\0';
   while( (ch = getch()) != 'q')
   {
      printf("> %c\r\n", ch);
   }
   printf("Tiny2 console build\r\n");
}
