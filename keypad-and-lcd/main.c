#include <string.h>
#include <stdio.h>
#include "stm32l476xx.h"
#include "SysClock.h"
#include "I2C.h"
#include "ssd1306.h"
#include "ssd1306_tests.h"
#include "keypad.h"

// Note: The code gets Warning L6989W
// Report: this is an known issue for STM32L4xx.
// https://www.keil.com/support/docs/3853.htm
// We can ignore this warning because we are not using
// external memory.

uint8_t Data_Receive[6];
uint8_t Data_Send[6];


void I2C_GPIO_init(void);
	
int main(void){
	volatile int i;
	int count = 0;
	
	int a = 0;
	int n = 0;
	
	char message[64] = "ABCDEFGHIJK";
	char chr;
	char str[64];
	
	
	System_Clock_Init(); // Switch System Clock = 80 MHz
	I2C_GPIO_init();
	I2C_Initialization(I2C1);

	//ssd1306_TestAll();
	ssd1306_Init();
	ssd1306_Fill(Black);
	ssd1306_SetCursor(2,0);
	//ssd1306_WriteString(message, Font_11x18, White);		
	//ssd1306_UpdateScreen();	
	
	
	
	
		Keypad_Pin_Init();
    ssd1306_Init();
    //buffer_init();
    //lcd_update();

    char last_key = NULL;

    while (1) {

        char key = keypad_scan();

        // --- debouncing + single press detection ---
        if (key != last_key) {

            if (key != 0xFF) {

                if (key == '*') {
                    //buffer_delete();
                } else {
                    //buffer_add(key);
                }

							char message2[1];
								message2[0] = key;
	ssd1306_Init();
	ssd1306_Fill(Black);
	ssd1306_SetCursor(2,0);
	ssd1306_WriteString(message2, Font_11x18, White);		
	ssd1306_UpdateScreen();	
            }
        }

        last_key = key;
        //delay_ms(20);   // debounce
    }
	
	
	
	
	while(1);	 // Deadloop
	
	
	
}

