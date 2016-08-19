#ifndef __UARTS_H
#define __UARTS_H

#include <stdio.h>
#include "stm32f10x.h"

void Initial_UART1(u32 baudrate);
void UART1_Put_Char(char DataToSend);
u8 UART1_Get_Char(void);
void UART1_Put_String(char *Str);

void Initial_UART2(u32 baudrate);
void UART2_Put_Char(char DataToSend);
u8 UART2_Get_Char(void);
void UART2_Put_String(char *Str);
#endif

//------------------End of File----------------------------

