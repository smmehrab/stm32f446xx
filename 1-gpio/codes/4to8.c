/*
INPUT PINS:
-----------

INPUT_1: 
PA_0
PA_1
PA_5
PA_6

INPUT_2:
PA_7
PA_8
PA_9
PA_10

OUTPUT PINS:
------------

PC_0
PC_1
PC_2
PC_3

PC_4
PC_5
PC_6
PC_8

OPCODE PIN:
------------

PA_14

*/

#include <stm32f446xx.h>

#define GPIOAEN       	(1U<<0)
#define GPIOBEN       	(1U<<1)
#define GPIOCEN         (1U<<2)

/*Function Protopypes*/

void GPIO_INIT(void);
void GPIOC_OUTPUT_MODE(int pin);
void SET_GPIOC(int pin);
void RESET_GPIOC(int pin);
void to_binary(int n, int nBin[]);

/*Function Definitions*/

void GPIO_INIT(void){
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
}

void SET_GPIOC(int pin){
	GPIOC->BSRR |= 1 << pin;
}

void RESET_GPIOC(int pin){
	GPIOC->BSRR |= 1 << (pin+16);
}

void to_binary(int n, int nBin[]){
	for(int i=0; i<8; i++){
		nBin[i] = n%2;
		n /= 2;
	}
}

int main(){
	
	GPIO_INIT();

	// Pin numbers of input1, input2 & output
	int INPUT1_PINS[4] = {0,1,5,6};
	int INPUT2_PINS[4] = {7,8,9,10};
	int OUTPUT_PINS[8] = {0,1,2,3,4,5,6,8};
	
	// Arrays for input1, input2 & output
	int INPUT1[4];
	int INPUT2[4];
	int OUTPUT[8];

	// Arithmetic variables
	int num1, num2, result, opcode;
	
	while(1){
		// RESET ALL
		for(int i=0; i<8; i++){
			OUTPUT[i] = 0;
			RESET_GPIOC(OUTPUT_PINS[i]);
		}
		opcode =0;
		
		// READ INPUT
		num1 = num2 = 0;
		for(int i=0; i<4; i++){
			INPUT1[i] = (GPIOA->IDR >> INPUT1_PINS[i]) & 1;
			INPUT2[i] = (GPIOA->IDR >> INPUT2_PINS[i]) & 1;
			num1 += INPUT1[i] << i;
			num2 += INPUT2[i] << i;
		}
		
		/* Arithmetic */
		// PA14 HIGH --> opcode 1 --> Multiply
		// PA14 LOW  --> opcode 0 --> Add
		opcode = (GPIOA->IDR >> 14) & 1;
		result = (opcode==1) ? (num1*num2) : (num1+num2);
		to_binary(result, OUTPUT);
		
		/* SET OUTPUT */
		for(int i=0; i<8; i++){
			if(OUTPUT[i]){
				SET_GPIOC(OUTPUT_PINS[i]);
			}
		}
	}
}