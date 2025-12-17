#include "keypad.h"

static const char keymap[4][4] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

void delay_ms(int ms) {
    for(volatile int i = 0; i < ms * 4000; i++);
}

void Keypad_Pin_Init(void) {
    // 1. Enable GPIOC Clock
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;

    //Output mode 01 PC0-PC3
    GPIOC->MODER &= ~(0xFFU << 0);
    GPIOC->MODER |=  (0x55U << 0);

    // Set rows initially HIGH
    GPIOC->ODR |= (0xFU << 0);

    //Input Mode 00 PC4, PC10, PC11, PC12
    GPIOC->MODER &= ~(3U << (4 * 2));
    GPIOC->MODER &= ~(3U << (10 * 2));
    GPIOC->MODER &= ~(3U << (11 * 2));
    GPIOC->MODER &= ~(3U << (12 * 2));

		//Pull-Up
    GPIOC->PUPDR &= ~(3U << (4 * 2));
		GPIOC->PUPDR |= (1U << (4 * 2));
    GPIOC->PUPDR &= ~(3U << (10 * 2));
		GPIOC->PUPDR |= (1U << (10 * 2));
    GPIOC->PUPDR &= ~(3U << (11 * 2));
		GPIOC->PUPDR |= (1U << (11 * 2));
    GPIOC->PUPDR &= ~(3U << (12 * 2));
		GPIOC->PUPDR |= (1U << (12 * 2));
}

char keypad_scan(void) {
    for (int row = 0; row < 4; row++) {
        // All rows HIGH first
        GPIOC->ODR |= (0xFU << 0); 
        
        // Set current row LOW
        GPIOC->ODR &= ~(1U << row);
        
        //delay_ms(100);

        int c1 = (GPIOC->IDR >> 4)  & 0x1;
        int c2 = (GPIOC->IDR >> 10) & 0x1;
        int c3 = (GPIOC->IDR >> 11) & 0x1;
        int c4 = (GPIOC->IDR >> 12) & 0x1;

        if (c1 == 0) return keymap[row][0];
        if (c2 == 0) return keymap[row][1];
        if (c3 == 0) return keymap[row][2];
        if (c4 == 0) return keymap[row][3];
    }
    
    // Set all rows back to HIGH
    GPIOC->ODR |= (0xFU << 0);
    
    return 0xFF; // No key pressed
}