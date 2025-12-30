#include <string.h>
#include <stdio.h>
#include "stm32l476xx.h"
#include "SysClock.h"
#include "I2C.h"
#include "ssd1306.h"
#include "ssd1306_tests.h"

#define SG90_PIN 0
#define HCSR04_PIN 6

// User HSI (high-speed internal) as the processor clock
void enable_HSI(){
	// Enable High Speed Internal Clock (HSI = 16 MHz)
  RCC->CR |= ((uint32_t)RCC_CR_HSION);
	
  // wait until HSI is ready
  while ( (RCC->CR & (uint32_t) RCC_CR_HSIRDY) == 0 ) {;}
	
  // Select HSI as system clock source 
  RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
  RCC->CFGR |= (uint32_t)RCC_CFGR_SW_HSI;  //01: HSI16 oscillator used as system clock

  // Wait till HSI is used as system clock source 
  while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) == 0 ) {;}
}

void Motor_Init(){
		RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
		GPIOA->MODER &= ~(3UL<<2*SG90_PIN);
		GPIOA->MODER |= (2UL<<2*SG90_PIN);
	
		GPIOA->AFR[0] &= ~(0xF <<4*SG90_PIN); //PIN numaramiz 0 ile 7 arasindaysa [0] digerlerinde [1]
		GPIOA->AFR[0] |= (2UL <<4*SG90_PIN); //tim5_ch1 PA0 icin alternate function 2 lazim, datasheetten
}


//SG90 TIMER
void TIM5_CH1_Init() {

	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM5EN;
	
	TIM5->CR1 &= ~TIM_CR1_DIR; //upcounting mode
	TIM5 -> PSC = 15; // Clock prescaler (16 bits, up to 65,535)
	TIM5 -> ARR = 19999; // Auto-reload: up-counting (0-->ARR), down-counting (ARR-->0)
	
	TIM5->CCMR1 &= ~TIM_CCMR1_OC1M;
	TIM5->CCMR1 |= TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1; //pwm mode 1 : 110
	
	TIM5->CCMR1 |= TIM_CCMR1_OC1PE; //preload enable
	
	TIM5->CCER &= ~TIM_CCER_CC1P; //default polarity
	TIM5->CCER |= TIM_CCER_CC1E;
	
	TIM5-> CCR1 = 1500; //initial 0 point
	
	TIM5-> CR1 |= TIM_CR1_CEN; //enable the counter

}


int current_ccr = 1500;
void Servo_SetAngle(int angle){
	//duty cycle = ccr / (arr+1) for pwm mode 1
	//ccr = 1000 for -90 rotate (1ms) but in real it's 500
	//ccr = 1500 for 0 rotate (1.5ms)
	//ccr = 2000 for 90 rotate (2ms) but in real it's 2500
	//1000/90 = 11, 1 degree for 11 ccr
	
	if(angle == -90)
		current_ccr = 500;
	else if(angle == 0)
		current_ccr = 1500;
	else if(angle == 90)
		current_ccr = 2500;
	
	
	else if(angle == 10 && current_ccr<=(2500-111) ) //+=10
		current_ccr += 111;
	else if(angle == -10 && current_ccr>=(500+111)) //-=10
		current_ccr -= 111;
	else if(angle) //for all values
		current_ccr = 1500 + angle * 11;
	
	TIM5->CCR1 = current_ccr; //duty cycle
}


void UltrasonicSensor_Init(){
		RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
		GPIOA->MODER &= ~(3UL<<2*HCSR04_PIN);
		GPIOA->MODER |= (2UL<<2*HCSR04_PIN);
	
		GPIOA->AFR[0] &= ~(0xF <<4*HCSR04_PIN); //PIN numaramiz 0 ile 7 arasindaysa [0] digerlerinde [1]
		GPIOA->AFR[0] |= (2UL <<4*HCSR04_PIN); //tim4_ch1 PA0 icin alternate function 2 lazim, datasheetten
}


//HCSR04 TIMER
void TIM4_CH1_Init() {
	
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM4EN;
	
	TIM4->CR1 &= ~TIM_CR1_DIR; //upcounting mode
	TIM4 -> PSC = 15; // Clock prescaler (16 bits, up to 65,535)
	TIM4 -> ARR = 0xFFFF; // Auto-reload: up-counting (0-->ARR), down-counting (ARR-->0)
	
	//Set the direction of channel 1 as input, and set input to 1, so value 01 in the CC1S fields in TIM4->CCMR1
	TIM4->CCMR1 &= ~TIM_CCMR1_CC1S;
	TIM4->CCMR1 |= TIM_CCMR1_CC1S_0;
	
	TIM4->CCMR1 &= ~TIM_CCMR1_IC1F;
	
	TIM4->CCER |= TIM_CCER_CC1P | TIM_CCER_CC1NP;
	
	TIM4->CCMR1 &= ~TIM_CCMR1_IC1PSC;
	
	TIM4->CCER |= TIM_CCER_CC1E;
	
	TIM4->DIER |= TIM_DIER_CC1IE;
		
	TIM4->DIER |= TIM_DIER_UIE;
	
	TIM4->CR1 |= TIM_CR1_CEN;

	NVIC_SetPriority(TIM4_IRQn, 0);
	
	NVIC_EnableIRQ(TIM4_IRQn);
}

		
int main(void){
		//System_Clock_Init();
		enable_HSI();
	
		I2C_GPIO_init();
		I2C_Initialization(I2C1);
	
		Motor_Init();
		TIM5_CH1_Init();
	
		Servo_SetAngle(0);
	
	
		char message[64] = "ABCDEFGHIJK";
		//ssd1306_TestAll();
		ssd1306_Init();
		ssd1306_Fill(Black);
		ssd1306_SetCursor(2,0);
		ssd1306_WriteString(message, Font_11x18, White);		
		ssd1306_UpdateScreen();	

		
	while(1){
		
			
		
	}

}
