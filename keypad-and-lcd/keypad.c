#include "keypad.h"

// Row pins: PC0, PC1, PC2, PC3  ? OUTPUT
// Col pins: PC4, PC10, PC11, PC12 ? INPUT (pull-up external)

static const char keymap[4][4] = {
    {'1','2','3','A'},
    {'4','5','6','B'},
    {'7','8','9','C'},
    {'*','0','#','D'}
};

void delay_ms(int ms) {
    for(int i = 0; i < ms * 4000; i++);
}

void Keypad_Pin_Init(void) {
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;

    // ROWS: PC0-PC3 OUTPUT
    GPIOC->MODER &= ~(0xFF << 0);
    GPIOC->MODER |=  (0x55 << 0);     // 01 01 01 01

    // COLS: PC4, PC10, PC11, PC12 INPUT
    // Input = 00
    GPIOC->MODER &= ~(3<<(4*2));    // PC4:  Clear bits 8-9
    GPIOC->MODER &= ~(3<<(10*2));   // PC10: Clear bits 20-21
    GPIOC->MODER &= ~(3<<(11*2));   // PC11: Clear bits 22-23
    GPIOC->MODER &= ~(3<<(12*2));   // PC12: Clear bits 24-25

    // No pull-up/down (external resistors used)
    //GPIOC->PUPDR &= ~(3<<(4*2));
    //GPIOC->PUPDR &= ~(3<<(10*2));
    //GPIOC->PUPDR &= ~(3<<(11*2));
    //GPIOC->PUPDR &= ~(3<<(12*2));
}

char keypad_scan(void) {

    // 1) Check if any key is pressed (set all rows LOW)
    //GPIOC->ODR &= ~0x0F;   // PC0-PC3 = 0
		GPIOC->ODR &= ~(1UL<<0); //PC0
		GPIOC->ODR &= ~(1UL<<1);
		GPIOC->ODR &= ~(1UL<<2);
		GPIOC->ODR &= ~(1UL<<3); //PC3

    //delay_ms(1);

    int col4 = (GPIOC->IDR >> 4) & 0x1;
    int col10 = (GPIOC->IDR >> 10) & 0x1;
    int col11 = (GPIOC->IDR >> 11) & 0x1;
    int col12 = (GPIOC->IDR >> 12) & 0x1;

   // if (col4 && col10 && col11 && col12)
     //   return 0xFF;   // no key


    // 2) Scan each row
    for (int row = 0; row < 4; row++) {

        // Set all rows high first
        //GPIOC->ODR |= 0x0F;
				GPIOC->ODR |= (1UL<<0); //PC0
				GPIOC->ODR |= (1UL<<1);
				GPIOC->ODR |= (1UL<<2);
				GPIOC->ODR |= (1UL<<3); //PC3

        // Set ONE row low
        GPIOC->ODR &= ~(1 << row);

        //delay_ms(1);

        int r_col4 = (GPIOC->IDR >> 4) & 0x1;
        int r_col10 = (GPIOC->IDR >> 10) & 0x1;
        int r_col11 = (GPIOC->IDR >> 11) & 0x1;
        int r_col12 = (GPIOC->IDR >> 12) & 0x1;

        if (r_col4 == 0) return keymap[row][0];
        else if (r_col10 == 0) return keymap[row][1];
        else if (r_col11 == 0) return keymap[row][2];
        else if (r_col12 == 0) return keymap[row][3];
    }

    return 0xFF;
}