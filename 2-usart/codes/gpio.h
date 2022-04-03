#include "stm32f446xx.h"

#define GPIOAEN       	(1U<<0)
#define GPIOBEN       	(1U<<1)
#define GPIOCEN         (1U<<2)

/*Function Protopypes*/

void GPIO_CONFIG(void);
void GPIOC_OUTPUT_MODE(int pin);
void SET_GPIOC(int pin);
void RESET_GPIOC(int pin);

/*Function Definitions*/

void GPIO_CONFIG(void) {
  // Enable clock for GPIO A and C
	RCC->AHB1ENR |= GPIOAEN;
	RCC->AHB1ENR |= GPIOCEN;
	
	// Set the output pins in output mode
	GPIOC_OUTPUT_MODE(0);
	GPIOC_OUTPUT_MODE(1);
	GPIOC_OUTPUT_MODE(2);
	GPIOC_OUTPUT_MODE(3);
	GPIOC_OUTPUT_MODE(4);
	GPIOC_OUTPUT_MODE(5);
	GPIOC_OUTPUT_MODE(6);
	GPIOC_OUTPUT_MODE(8);
}

void GPIOC_OUTPUT_MODE(int pin){
	GPIOC->MODER |= 1 << (pin*2);
	// 0 --> push pull
	// GPIOC->OTYPER &= ~(1<<pin);
	// 10 --> Fast Speed
	// GPIOC->OSPEEDR |= (1<<((2*pin)+1));   
	// 00 --> no pull up or pulldown
	// GPIOC->PUPDR &= ~((1<<(2*pin) | (1<<((2*pin)+1))));  
}

void SET_GPIOC(int pin){
	GPIOC->BSRR |= 1 << pin;
}

void RESET_GPIOC(int pin){
	GPIOC->BSRR |= 1 << (pin+16);
}
