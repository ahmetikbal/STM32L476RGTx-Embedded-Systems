#ifndef __KEYPAD_H
#define __KEYPAD_H

#include "stm32l476xx.h"

void Keypad_Pin_Init(void);
char keypad_scan(void);     // Tek tus tespiti (row+col)
void delay_ms(int ms);      // Basit delay

#endif