/*
INPUT PINS:
-----------
INPUT_1: PC_0
INPUT_2: PC_1

OUTPUT PINS:
------------
OUTPUT_1: PB_5
OUTPUT_2: PB_4
OUTPUT_3: PB_10
OUTPUT_4: PA_8
*/

#include "stm32f446xx.h"

#define GPIOAEN       	(1U<<0)
#define GPIOBEN       	(1U<<1)
#define GPIOCEN         (1U<<2)

#define INPUT_1         (1U<<0)
#define INPUT_2					(1U<<1)

#define OUTPUT_1        (1U<<5)
#define OUTPUT_2        (1U<<4)
#define OUTPUT_3        (1U<<10)
#define OUTPUT_4        (1U<<8)

/*Function Protopypes*/

void GPIO_INIT(void);

void SET_OUTPUT1(void);
void RESET_OUTPUT1(void);

void SET_OUTPUT2(void);
void RESET_OUTPUT2(void);

void SET_OUTPUT3(void);
void RESET_OUTPUT3(void);

void SET_OUTPUT4(void);
void RESET_OUTPUT4(void);

void RESET_ALL(void);

/*Function Definitions*/

void GPIO_INIT(void){
	
	/* CLOCK */
	
  // Enable clock for GPIO A,B and C
	RCC->AHB1ENR |= GPIOAEN;
	RCC->AHB1ENR |= GPIOBEN;
	RCC->AHB1ENR |= GPIOCEN;

	/* INPUT */
	
	// Set GPIOC pin 0 as INPUT (input 1)
	GPIOA->MODER &=~(1U<<0);
	GPIOA->MODER &=~(1U<<1);
	
	// Set GPIOC pin 1 as INPUT (input 2)
	GPIOA->MODER &=~(1U<<2);
	GPIOA->MODER &=~(1U<<3);
	
	/* OUTPUT */

	// Set GPIOB pin 5 as OUTPUT (output 1)
	GPIOB->MODER |= (1U<<10);
	GPIOB->MODER &=~(1U<<11);
	
	// Set GPIOB pin 4 as OUTPUT (output 2)
	GPIOB->MODER |= (1U<<8);
	GPIOB->MODER &=~(1U<<9);
	
	// Set GPIOB pin 10 as OUTPUT (output 3)
	GPIOB->MODER |= (1U<<20);
	GPIOB->MODER &=~(1U<<21);
	
	// Set GPIOA pin 8 as OUTPUT (output 4)
	GPIOA->MODER |= (1U<<16);
	GPIOA->MODER &=~(1U<<17);
}

// Functions for setting and resetting the output pins
void SET_OUTPUT1(void){
	GPIOB->BSRR |= OUTPUT_1;
}

void RESET_OUTPUT1(void){
	GPIOB->BSRR |= OUTPUT_1<<16;
}

void SET_OUTPUT2(void){
	GPIOB->BSRR |= OUTPUT_2;
}

void RESET_OUTPUT2(void){
	GPIOB->BSRR |= OUTPUT_2<<16;
}

void SET_OUTPUT3(void){
	GPIOB->BSRR |= OUTPUT_3;
}

void RESET_OUTPUT3(void){
	GPIOB->BSRR |= OUTPUT_3<<16;
}

void SET_OUTPUT4(void){
	GPIOA->BSRR |= OUTPUT_4;
}

void RESET_OUTPUT4(void){
	GPIOA->BSRR |= OUTPUT_4<<16;
}

void RESET_ALL(void){
	RESET_OUTPUT1();
	RESET_OUTPUT2();
	RESET_OUTPUT3();
	RESET_OUTPUT4();
}


int main(void){
	
	GPIO_INIT();
	
	while(1){
		
		// Reading Input
		uint16_t input1 = GPIOC->IDR & INPUT_1;
		uint16_t input2 = GPIOC->IDR & INPUT_2;
		
		/*
		Truth Table (2-to-4)
		-------------------------
		| input1 | input2 | LED |
		-------------------------
		|   0    |   0    |  1  |
		-------------------------
		|   0    |   1    |  2  |
		-------------------------
		|   1    |   0    |  3  |
		-------------------------
		|   1    |   1    |  4  |
		-------------------------	
		*/
		
		if(!input1 && !input2 ){
			RESET_ALL();
			SET_OUTPUT1();
		}
		else if(!input1 && input2){
			RESET_ALL();
			SET_OUTPUT2();
		}
		else if(input1 && !input2){
			RESET_ALL();
			SET_OUTPUT3();
		}
		else{
			RESET_ALL();
			SET_OUTPUT4();
		}
	}
}

