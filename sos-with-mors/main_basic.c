#include "stm32l476xx.h"

// PA.5  <--> Green LED
// PC.13 <--> Blue user button
#define LED_PIN    5
#define BUTTON_PIN 13


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


void configure_LED_pin(){
	// PA.5  <--> Green LED
  // Enable the clock to GPIO Port A
	//RCC->AHB2ENR |= (1<<2);	
  RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;   
		
	// GPIO Mode: Input(00), Output(01), AlterFunc(10), Analog(11, reset)
	GPIOA->MODER &= ~(3UL<<(2*LED_PIN));  
	GPIOA->MODER |=   1UL<<(2*LED_PIN);      // Output(01)
	
	// GPIO Speed: Low speed (00), Medium speed (01), Fast speed (10), High speed (11)
	GPIOA->OSPEEDR &= ~(3<<(2*LED_PIN));
	GPIOA->OSPEEDR |=   2<<(2*LED_PIN);  // Fast speed
	
	// GPIO Output Type: Output push-pull (0, reset), Output open drain (1) 
	GPIOA->OTYPER &= ~(1<<LED_PIN);      // Push-pull
	
	// GPIO Push-Pull: No pull-up, pull-down (00), Pull-up (01), Pull-down (10), Reserved (11)
	GPIOA->PUPDR  &= ~(3<<(2*LED_PIN));  // No pull-up, no pull-down
}


void configure_button_pin(){
	// PC.13 <--> Blue user button
  // Enable the clock to GPIO Port C	
  RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;   
		
	// GPIO Mode: Input(00), Output(01), AlterFunc(10), Analog(11, reset)
  GPIOC->MODER &= ~(3UL<<26);              // Input(00)
	
	// GPIO Push-Pull: No pull-up, pull-down (00), Pull-up (01), Pull-down (10), Reserved (11)
  GPIOC->PUPDR &= ~(3UL<<26);              // No pull-up, no pull-down
}

// Input: ticks = number of ticks between two interrupts
void SysTick_Initialize (uint32_t ticks) {
SysTick->CTRL = 0; // Disable SysTick
SysTick->LOAD = ticks - 1; // Set reload register
// Set interrupt priority of SysTick to least urgency (i.e., largest priority value)
NVIC_SetPriority (SysTick_IRQn, (1<<__NVIC_PRIO_BITS) - 1);
SysTick->VAL = 0; // Reset the SysTick counter value
// Select processor clock: 1 = processor clock; 0 = external clock
SysTick->CTRL |= (1UL<<2);
// Enables SysTick interrupt, 1 = Enable, 0 = Disable
SysTick->CTRL |= (1UL<<1);
// Enable SysTick
SysTick->CTRL |= (1UL);
}


void turn_on_LED(){
	GPIOA->ODR |= 1 << LED_PIN;
}

void turn_off_LED(){
	GPIOA->ODR &= ~(1 << LED_PIN);
}

void toggle_LED(){
	GPIOA->ODR ^= (1 << LED_PIN);
}


volatile int32_t TimeDelay;
void Delay (uint32_t nTime) {
// nTime: specifies the delay time length
TimeDelay = nTime; // TimeDelay must be declared as volatile
while(TimeDelay != 0); // Busy wait
}


int main (void){
SysTick_Initialize(4000000); // Interrupt period = 4000000 cycles

enable_HSI();
configure_LED_pin();
configure_button_pin();
	
	while(1){
	 if ((GPIOC->IDR & GPIO_IDR_IDR_13) == 0) { // PC13 düsükse, butona basildi demek
			turn_off_LED();
		 
		 	for(int j =0;j<6;j++){
							toggle_LED();
							Delay(1); // Delay 1 ticks
						}

					  for(int j =0;j<6;j++){
							toggle_LED();
							if(j%2 == 0)
								Delay(2);
							else
								Delay(1);
						}
						
						
						for(int j =0;j<6;j++){
							toggle_LED();
							Delay(1);
						}
						
				for (int i = 0; i < 1000000; i++); // Debouncing için gecikme

	 }
 }
	
}

void SysTick_Handler (void) { // SysTick interrupt service routine
if (TimeDelay > 0) // Prevent it from being negative
TimeDelay--; // TimeDelay is a global volatile variable
}