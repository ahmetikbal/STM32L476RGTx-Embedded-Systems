#ifndef __KEYPAD_H
#define __KEYPAD_H

#include "stm32l476xx.h"

void Keypad_Pin_Init(void);
char keypad_scan(void);
void delay_ms(int ms);

#endif