#include "stm32f446xx.h"

/*Function Protopypes*/

void TIMER_CONFIG(void);
void DELAY_MICRO(uint16_t us);
void DELAY(uint16_t ms);

/*Function Definitions*/

void TIMER_CONFIG(void) {
	// 1. Enable Timer6
	RCC->APB1ENR |= (1<<4);
	
	// 2. Set the prescalar and the ARR
	TIM6->PSC = 90-1;  // 90MHz/90 = 1 MHz ~~ 1 microsecond delay
	TIM6->ARR = 0xffff;  // MAX ARR value
	
	/* 3. Enable the Timer and Wait for the Update Flag (UIF) to Set */
	// Enable Counter
	TIM6->CR1 |= (1<<0); 
	// UIF bit is set by hardware when the registers are updated
	while (!(TIM6->SR & (1<<0)));  
}

// Delay in Microseconds
void DELAY_MICRO(uint16_t microseconds){
	TIM6->CNT = 0;
	while (TIM6->CNT < microseconds);
}

// Delay in Milliseconds
void DELAY(uint16_t ms){
	for (uint16_t i=0; i<ms; i++){
		// 1000 microseconds = 1 millisecond
		DELAY_MICRO (1000);
	}
}
