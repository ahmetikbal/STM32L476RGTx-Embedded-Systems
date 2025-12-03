#include "stm32l476xx.h"
#include "SysClock.h"
#include "I2C.h"
#include "ssd1306.h"
#include "keypad.h"

void I2C_GPIO_init(void);

#define BUFFER_SIZE 16

int main(void){
    System_Clock_Init();
    I2C_GPIO_init();
    I2C_Initialization(I2C1);

    ssd1306_Init();
    ssd1306_Fill(Black);

    Keypad_Pin_Init();

    char keyBuffer[BUFFER_SIZE + 1] = {0}; // +1 for null terminator
    int bufferIndex = 0;
    
    // Initialize buffer with spaces
    for(int i = 0; i < BUFFER_SIZE; i++) {
        keyBuffer[i] = ' ';
    }
    keyBuffer[BUFFER_SIZE] = '\0';

    char lastKey = 0xFF; //for debouncing

    while (1) {
        char key = keypad_scan();
        
        if (key != lastKey) {
            if (key != 0xFF) {
                
                if (key == '*') { //Delete
                    if (bufferIndex > 0) {
                        bufferIndex--;
                        keyBuffer[bufferIndex] = ' ';
                    }
                } else {
                    if (bufferIndex < BUFFER_SIZE) {
                        keyBuffer[bufferIndex] = key;
                        bufferIndex++;
                    }
                }
                
                ssd1306_Fill(Black);
                ssd1306_SetCursor(2, 0);
                ssd1306_WriteString(keyBuffer, Font_11x18, White);
                ssd1306_UpdateScreen();
            }
            
            lastKey = key;
        }
        
        delay_ms(10);
    }
}