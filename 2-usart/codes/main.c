/*
Command Examples
----------------
config temp 25 water 50 hum 45 light 1111!
read config temp!
config light 1101!

Command Delimiter
-----------------
'!' (Every command must end with this character inorder for this program to detect the end of the command.)

USART2 PINS
-----------
PA_2
PA_3

Temp
----

INPUT PINS:
----------- 
PA_0
PA_1

OUTPUT PINS:
------------
PC_0 (Cooler)
PC_1 (Heater)

Water
-----

INPUT PINS:
----------- 
PA_5
PA_6

OUTPUT PINS:
------------
PC_2 (Pump)

Hum
---

INPUT PINS:
----------- 
PA_7
PA_8

OUTPUT PINS:
------------
PC_3	(HumAdjuster)

Light
-----

INPUT PINS:
-----------
PA_14
PA_9
PA_10

OUTPUT PINS:
------------
PC_4	(Living)
PC_5	(Dining)
PC_6	(Bedroom)
PC_8	(Office)

*/

#include "stm32f4xx.h"
#include "clock.h"
#include "timer.h"
#include "usart.h"
#include "gpio.h"

#include <stdint.h>
#include <stdio.h>
#include <stdio.h>
#include <string.h>

#define GPIOAEN			(1U<<0)
#define UART2EN			(1U<<17)

#define CR1_TE			(1U<<3)
#define CR1_RE			(1U<<2)

#define CR1_UE			(1U<<13)
#define SR_TXE			(1U<<7)
#define SR_RXNE			(1U<<5)
#define CR1_RXNEIE		(1U<<5)

#define SYS_FREQ		45000000
#define APB1_CLK		SYS_FREQ

#define UART_BAUDRATE		115200

/* Global Variables */
	
static char command[100];
static int commandLength=0;
static int commandType=0;
// 0 --> Config Update
// 1 --> Read Config

static int temp = -1;
static int water = -1;
static int hum = -1;
static int light = -1;
static int adjustmentPeriod = 3; // (seconds)

/*Function Protopypes*/

void COMMAND_HANDLER(void);
void USART2_IRQHandler(void);
static void USART_CALLBACK(void);

/*Function Definitions*/

void COMMAND_HANDLER(void) {
	commandType = 0;
		int i=0;
		while(i<commandLength) {
			if(i+3 <commandLength && command[i] == 'r' && command[i+1] == 'e' && command[i+2] == 'a' && command[i+3] == 'd') {
				commandType = 1;
				i += 4;
			}
			else if(i+3 <commandLength && command[i] == 't' && command[i+1] == 'e' && command[i+2] == 'm' && command[i+3] == 'p') {
				i += 4;
				if(commandType==1) {
					// Show Temp Config
					USART2_SEND_STRING("Temp:\n\r");
					USART2_SEND_INT(temp);
					USART2_SEND_STRING("\n\r");
				}
				else{
					i++;
					// Parse value for temp command
					temp = 0;
					while(i<commandLength && command[i]!=' ' && command[i]!='!') {
						temp = temp * 10;
						temp += (command[i] - '0');
						i++;
					}
				}
			}
			else if(i+4 <commandLength && command[i] == 'w' && command[i+1] == 'a' && command[i+2] == 't' && command[i+3] == 'e' && command[i+4] == 'r') {
				i += 5;
				if(commandType==1) {
					// Show Water Config
					USART2_SEND_STRING("Water:\n\r");
					USART2_SEND_INT(water);
					USART2_SEND_STRING("\n\r");
				}
				else{
					i++;
					// Parse value for water command
					water = 0;
					while(i<commandLength && command[i]!=' ' && command[i]!='!') {
						water = water * 10;
						water += (command[i] - '0');
						i++;
					}
				}
			}
			else if(i+2 <commandLength && command[i] == 'h' && command[i+1] == 'u' && command[i+2] == 'm') {
				i += 3;
				if(commandType==1) {
					// Show Hum Config
					USART2_SEND_STRING("Hum:\n\r");
					USART2_SEND_INT(hum);
					USART2_SEND_STRING("\n\r");
				}				
				else{
					i++;
					// Parse value for hum command
					hum = 0;
					while(i<commandLength && command[i]!=' ' && command[i]!='!') {
						hum = hum * 10;
						hum += (command[i] - '0');
						i++;
					}
				}
			}
			else if(i+4 <commandLength && command[i] == 'l' && command[i+1] == 'i' && command[i+2] == 'g' && command[i+3] == 'h' && command[i+4] == 't') {
				i += 5;
				if(commandType==1) {
					// Show Light Config
					USART2_SEND_STRING("Light:\n\r");
					USART2_SEND_INT(light);
					USART2_SEND_STRING("\n\r");
				}				
				else{
					i++;
					// Parse value for light command
					light = 0;
					while(i<commandLength && command[i]!=' ' && command[i]!='!') {
						light = light * 10;
						light += (command[i] - '0');
						i++;
					}
				}
			}
			else {
				i++;
			}
		}
		
		// Show Updated Config Variables
		if(commandType==0) {
			USART2_SEND_STRING("Temp:\n\r");
			USART2_SEND_INT(temp);
			USART2_SEND_STRING("\n\r");
			USART2_SEND_STRING("Water:\n\r");
			USART2_SEND_INT(water);
			USART2_SEND_STRING("\n\r");
			USART2_SEND_STRING("Hum:\n\r");
			USART2_SEND_INT(hum);
			USART2_SEND_STRING("\n\r");
			USART2_SEND_STRING("Light:\n\r");
			USART2_SEND_INT(light);
			USART2_SEND_STRING("\n\r");
		}
} 

static void USART_CALLBACK(void) {
	char key = USART2->DR;
	command[commandLength++] = key;
	if(key=='!') {
		// Show Received Command
		USART2_SEND_STRING("\n\rCommand Received:\n\r");
		USART2_SEND_STRING(command);
		USART2_SEND_STRING("\n");
		
		// Parse Configuration/Command & Provide Responses
		COMMAND_HANDLER();
		
		// Reset Command
		memset(command, 0, commandLength);
		commandLength = 0;
	}
}

void USART2_IRQHandler(void){
	// Check if RXNE is set
	if(USART2->SR & SR_RXNE) {
		USART_CALLBACK();
	}
}

int main (void) {
	CLOCK_CONFIG();
	TIMER_CONFIG();
	GPIO_CONFIG();
	USART2_RX_INTERRUPT_CONFIG();
	
	// Pin Numbers
	int TEMP_INPUT_PINS[2] = {0,1};
	int TEMP_OUTPUT_PINS[2] = {0, 1};
	int WATER_INPUT_PINS[2] = {5,6};
	int WATER_OUTPUT_PINS[1] = {2};
	int HUM_INPUT_PINS[2] = {7,8};
	int HUM_OUTPUT_PINS[1] = {3};
	int LIGHT_INPUT_PINS[3] = {14,9,10};
	int LIGHT_OUTPUT_PINS[4] = {4,5,6,8};

	// RESET ALL OUTPUT
	RESET_GPIOC(0);
	RESET_GPIOC(1);
	RESET_GPIOC(2);
	RESET_GPIOC(3);
	RESET_GPIOC(4);
	RESET_GPIOC(5);
	RESET_GPIOC(6);
	RESET_GPIOC(8);

	/* Home Automation Variables */
	int tempStatus1, tempStatus2;
	int humStatus1, humStatus2;
	int waterStatus1, waterStatus2;
	int lightStatus1, lightStatus2, lightStatus3;

	/* Home Automation Loop */
	while(1){
		/* Temperature Control*/
		tempStatus1 = (GPIOA->IDR >> TEMP_INPUT_PINS[0]) & 1;
		tempStatus2 = (GPIOA->IDR >> TEMP_INPUT_PINS[1]) & 1;
		
		// Within Range
		if(tempStatus1==0 && tempStatus2==0 && temp != -1) {
			RESET_GPIOC(TEMP_OUTPUT_PINS[0]);
			RESET_GPIOC(TEMP_OUTPUT_PINS[1]);
		}
		// Above
		else if(tempStatus1==1 && tempStatus2==1 && temp != -1){
			// Heater Off
			RESET_GPIOC(TEMP_OUTPUT_PINS[1]);
			// Cooler On
			SET_GPIOC(TEMP_OUTPUT_PINS[0]); 
			DELAY(adjustmentPeriod*1000);
			RESET_GPIOC(TEMP_OUTPUT_PINS[0]);
		}
		// Below
		else if(tempStatus1!=tempStatus2 && temp != -1){
			// Cooler Off
			RESET_GPIOC(TEMP_OUTPUT_PINS[0]);
			// Heater On
			SET_GPIOC(TEMP_OUTPUT_PINS[1]);
			DELAY(adjustmentPeriod*1000);
			RESET_GPIOC(TEMP_OUTPUT_PINS[1]);			
		}
		
		/* Water Control*/
		waterStatus1 = (GPIOA->IDR >> WATER_INPUT_PINS[0]) & 1;
		waterStatus2 = (GPIOA->IDR >> WATER_INPUT_PINS[1]) & 1;
		
		// Above
		if(waterStatus1==1 && waterStatus2==1 && water!=-1){
			// Pump Off
			RESET_GPIOC(WATER_OUTPUT_PINS[0]);
		}
		// Below
		else if(waterStatus1!=waterStatus2 && water!=-1){
			// Pump On
			SET_GPIOC(WATER_OUTPUT_PINS[0]);
			DELAY(adjustmentPeriod*1000);
			RESET_GPIOC(WATER_OUTPUT_PINS[0]);	
		}		
		
		/* Humidity Control*/
		humStatus1 = (GPIOA->IDR >> HUM_INPUT_PINS[0]) & 1;
		humStatus2 = (GPIOA->IDR >> HUM_INPUT_PINS[1]) & 1;
		
		// Above
		if(humStatus1==1 && humStatus2==1 && hum!=-1){
			// HumAdjuster Off
			RESET_GPIOC(HUM_OUTPUT_PINS[0]);
		}
		// Below
		else if(humStatus1!=humStatus2 && hum!=-1){
			// HumAdjuster On
			SET_GPIOC(HUM_OUTPUT_PINS[0]);
			DELAY(adjustmentPeriod*1000);
			RESET_GPIOC(HUM_OUTPUT_PINS[0]);	
		}
		
		/* Light Control*/
		lightStatus1 = (GPIOA->IDR >> LIGHT_INPUT_PINS[0]) & 1;
		lightStatus2 = (GPIOA->IDR >> LIGHT_INPUT_PINS[1]) & 1;
		lightStatus3 = (GPIOA->IDR >> LIGHT_INPUT_PINS[2]) & 1;
		
		// Night Time
		if(lightStatus1==1 && light!=-1) {
			// Reset All Rooms' Light
			RESET_GPIOC(LIGHT_OUTPUT_PINS[0]);
			RESET_GPIOC(LIGHT_OUTPUT_PINS[1]);
			RESET_GPIOC(LIGHT_OUTPUT_PINS[2]);
			RESET_GPIOC(LIGHT_OUTPUT_PINS[3]);
			// Living Room
			if(lightStatus2 == 0 && lightStatus3 == 0){
				if((light/1000)%2==1) {
					SET_GPIOC(LIGHT_OUTPUT_PINS[0]);
				}
			}
			// Dining Room
			else if(lightStatus2 == 0 && lightStatus3 == 1){
				if((light/100)%2==1) {
					SET_GPIOC(LIGHT_OUTPUT_PINS[1]);
				}			
			}
			// Bed Room
			else if(lightStatus2 == 1 && lightStatus3 == 0){
				if((light/10)%2==1) {
					SET_GPIOC(LIGHT_OUTPUT_PINS[2]);
				}		
			}
			// Office Room
			else if(lightStatus2 == 1 && lightStatus3 == 1){
				if(light%10 == 1) {
					SET_GPIOC(LIGHT_OUTPUT_PINS[3]);
				}		
			}
		}
		
		// Day Time
		else if(lightStatus1==0 && light!=-1) {
			// All Rooms' Lights Off
			RESET_GPIOC(LIGHT_OUTPUT_PINS[0]);
			RESET_GPIOC(LIGHT_OUTPUT_PINS[1]);
			RESET_GPIOC(LIGHT_OUTPUT_PINS[2]);
			RESET_GPIOC(LIGHT_OUTPUT_PINS[3]);
		}
	}
}
