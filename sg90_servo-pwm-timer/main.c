#include <string.h>
#include <stdio.h>
#include "stm32l476xx.h"
#include "SysClock.h"
#include "keypad.h"

#define SG90_PIN 0


void Motor_Init(){
		RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
		GPIOA->MODER &= ~(3UL<<2*SG90_PIN);
		GPIOA->MODER |= (2UL<<2*SG90_PIN);
	
		GPIOA->AFR[0] &= ~(0xF <<4*SG90_PIN); //PIN numaramiz 0 ile 7 arasindaysa [0] digerlerinde [1]
		GPIOA->AFR[0] |= (2UL <<4*SG90_PIN); //tim5_ch1 PA0 icin alternate function 2 lazim, datasheetten
}


void TIM5_CH1_Init() {

	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM5EN;
	
	TIM5->CR1 &= ~TIM_CR1_DIR; //upcounting mode
	TIM5 -> PSC = 3; // Clock prescaler (16 bits, up to 65,535)
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
	
	
	TIM5->CCR1 = current_ccr; //duty cycle
}

		
int main(void){
		//System_Clock_Init();
		Keypad_Pin_Init();
		Motor_Init();
		TIM5_CH1_Init();
	
		Servo_SetAngle(0);

		
	while(1){
		
		char key = keypad_scan();
		
		if(key == 'A')
			Servo_SetAngle(-10);
		else if(key == 'B')
			Servo_SetAngle(10);
		else if(key == 'C')
			Servo_SetAngle(0);
		
		
		//test
		else if(key == '3')
			Servo_SetAngle(-90);
		else if(key == '6')
			Servo_SetAngle(90);
		
	}

}