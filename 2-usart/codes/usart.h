#include <stdint.h>
#include "stm32f4xx.h"

#define GPIOAEN			(1U<<0)
#define USART2EN		(1U<<17)

#define CR1_TE			(1U<<3)
#define CR1_RE			(1U<<2)

#define CR1_UE			(1U<<13)
#define SR_TXE			(1U<<7)
#define SR_RXNE			(1U<<5)
#define CR1_RXNEIE	(1U<<5)

#define SYS_FREQ		45000000
#define APB1_CLK		SYS_FREQ

#define USART_BAUDRATE		115200

/*Function Protopypes*/

void USART2_RX_INTERRUPT_CONFIG(void);
char USART2_GET_CHAR(void);
void USART2_SEND_CHAR(uint8_t c);
void USART2_SEND_STRING(char *string);
void USART2_SEND_INT(int n);
static void USART_SET_BAUDRATE(USART_TypeDef *USARTx, uint32_t PeriphClk,  uint32_t BaudRate);
static uint16_t COMPUTE_USART_BAUDRATE(uint32_t PeriphClk, uint32_t BaudRate);

/*Function Definitions*/

void USART2_RX_INTERRUPT_CONFIG(void) {
	
	/****************Configure USART2 GPIO Pins***************/
	
	// Enable clock access to gpioa
	RCC->AHB1ENR |= GPIOAEN;
	// GPIOA high speed mode
	GPIOA->OSPEEDR |= (3<<4) | (3<<6);

	// Set PA2 mode to alternate function mode 
	GPIOA->MODER &=~(1U<<4);
	GPIOA->MODER |= (1U<<5);

	// Set PA2 alternate function type to UART_TX (AF07)
	GPIOA->AFR[0] |= (1U<<8);
	GPIOA->AFR[0] |= (1U<<9);
	GPIOA->AFR[0] |= (1U<<10);
	GPIOA->AFR[0] &= ~(1U<<11);
	
	// Set PA3 mode to alternate function mode
	GPIOA->MODER &=~(1U<<6);
	GPIOA->MODER |= (1U<<7);
	
	// Set PA3 alternate function type to UART_RX (AF07)
	GPIOA->AFR[0] |= (1U<<12);
	GPIOA->AFR[0] |= (1U<<13);
	GPIOA->AFR[0] |= (1U<<14);
	GPIOA->AFR[0] &= ~(1U<<15);

	/****************Configure USART2 Module****************/
	
	// Enable clock access to uart2
	RCC->APB1ENR |= USART2EN;

	// Configure baudrate
	USART_SET_BAUDRATE(USART2, APB1_CLK, USART_BAUDRATE);

	// Configure the transfer direction
	USART2->CR1 =  (CR1_TE | CR1_RE);

	// Enable RXNE inteerupt
	USART2->CR1 |=CR1_RXNEIE;

	// Enable USART2 Interrupt in NVIC
	NVIC_EnableIRQ(USART2_IRQn);

	// Enable USART Module
	USART2->CR1 |= CR1_UE;
}


char USART2_GET_CHAR(void) {
	// Make sure the receive data register is not empty
	while(!(USART2->SR & SR_RXNE)){}
	// Read data
	return USART2->DR;
}

void USART2_SEND_CHAR(uint8_t c) {
  // Write to transmit data register
	USART2->DR	=  c;
	 // Make sure the transmit data register is empty
	while(!(USART2->SR & SR_TXE)){}
}

void USART2_SEND_STRING(char *string) {
	while(*string) USART2_SEND_CHAR(*string++);
}

void USART2_SEND_INT(int n){
	if(n>0) {
		char c = (n%10) + '0';
		USART2_SEND_INT(n/10);
		USART2_SEND_CHAR(c);
	}
	else if(n<0) {
		USART2_SEND_STRING("-1");
	}
}

static void USART_SET_BAUDRATE(USART_TypeDef *USARTx, uint32_t PeriphClk,  uint32_t BaudRate) {
	USARTx->BRR =  COMPUTE_USART_BAUDRATE(PeriphClk,BaudRate);
}

static uint16_t COMPUTE_USART_BAUDRATE(uint32_t PeriphClk, uint32_t BaudRate) {
	return ((PeriphClk + (BaudRate/2U))/BaudRate);
}
