/* Standard STM32F4 library includes */

#include <stm32f4xx.h>
#include <misc.h>			
#include <stm32f4xx_usart.h> 
#include <string.h>
#include <stdio.h>
#define MAX_LENGTH 8

char message[MAX_LENGTH + 1];
volatile int counter = 0;

double getTempValue(void);

/* This function is used to transmit a string of characters via
 * the USART specified in USARTx.
 *
 * It takes two arguments: USARTx --> can be any of the USARTs e.g. USART1, USART6 etc.
 * 						   (volatile) char *s is the string you want to send
 *
 * Note: The string has to be passed to the function as a pointer because
 * 		 the compiler doesn't know the 'string' data type. In standard
 * 		 C a string is just an array of characters
 *
 * Note 2: At the moment it takes a volatile char because the received_string variable
 * 		   declared as volatile char --> otherwise the compiler will spit out warnings
 * */
void USART_puts(USART_TypeDef* USARTx, volatile char *s){

	while(*s){
		// wait until data register is empty
		while( !(USARTx->SR & 0x00000040) );
		USART_SendData(USARTx, *s);
		*s++;
	}
}

int checkMessage() {
	message[counter] = '\0';
	if (!strcmp("temp", message)) {
		return 1;
	}
	return 0;
}

void sendDataToObserver() {
	char data[10];
	sprintf(data, "T = %.2lf \r\n", getTempValue());
	USART_puts(USART1, data);
	USART_puts(USART6, "\r\nRequested data sent to observer!\r\n\n");
}

/**
* This function is designed for initializing given USART1 with
* given baudrate.
*
* @param baudrate given baudrate
* */
void init_USART1(int baudrate) {
	
	/* GPIO struktura za inicijalizaciju */
	GPIO_InitTypeDef GPIO_InitStruct;
	/* USART struktura za inicijalizaciju */
	USART_InitTypeDef USART_InitStruct;
	/* NVIC struktura za inicijalizaciju */
	NVIC_InitTypeDef NVIC_InitStruct;

	/* Za USART1 omogucavamo clock na APB2 sabirnice*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE); 
	 
	/* Ukljucivanje clocka za GPIOB na AHB1 sabirnici */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);	
	/* Formiranje GPIO strukture */
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; //pin 6(TX), pin7(RX) 
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF; 			
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;			
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	/* Delegiranje strukture funkciji GPIO_Init koja obavlja inicijalizaciju */
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	/* Povezivanje GPIO pinova s USART1. USART1 preuzima kontrolu nad pinovima*/
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_USART1); 
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_USART1);

	/* Formiranje USART strukture postavljanjem razlicitih postavki */
	USART_InitStruct.USART_BaudRate = baudrate;				
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;		
	USART_InitStruct.USART_Parity = USART_Parity_No;		
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx; 
	
	/* Delegiranje strukture funkciji USART_Init koja obavlja inicijalizaciju */
	USART_Init(USART1, &USART_InitStruct);				
	
	/* Omogucavanje prekida za USART1 */
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); 
	
	/* Formiranje NVIC strukture */
	NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn; //zadavanje prekidnog vektora
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;// postavljanje prioriteta
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;	// postavljanje subpriortiteta
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;	// omogucavanje prekida globalno
	
	/* Delegiranje strukture funkciji NVIC_Init koja obavlja inicijalizaciju */
	NVIC_Init(&NVIC_InitStruct);							

	/* Omogucavanje rada za USART1 */
	USART_Cmd(USART1, ENABLE);
}

/**
* This function is designed for initializing given USART6 with
* given baudrate.
*
* @param USART6 given USART to initialize
* @param baudrate given baudrate
* */
void init_USART6(int baudrate) {
	
	/* GPIO structure for initialization */
	GPIO_InitTypeDef GPIO_InitStruct;
	/* USART structure for initialization */
	USART_InitTypeDef USART_InitStruct;
	/* NVIC structure for initialization */
	NVIC_InitTypeDef NVIC_InitStruct;
	
	/* For USART1 and USART6 APB2 have to be enabled, APB1 for USART6
	 * and USART3. 
	 * */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);
	 
	/* Enables the peripheral clock for the pins used by
	 * USART on GPIOB.
	 */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);	
	
	/* Initialization of GPIO structure with collected
	 * data.
	 */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; // Pins 6 (TX) and 7 (RX) are used
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF; 			// the pins are configured as alternate function so the USART peripheral has access to them
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;		// this defines the IO speed and has nothing to do with the baudrate!
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;			// this defines the output type as push pull mode (as opposed to open drain)
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;			// this activates the pull-up resistors on the IO pins
	GPIO_Init(GPIOC, &GPIO_InitStruct);

	/* The RX and TX pins are now connected to their AF
	 * so that the USART1 can take over control of the
	 * pins
	 */
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_USART6); //
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_USART6);

	/* Now the USART_InitStruct is used to define the
	 * properties of USART1
	 */
	USART_InitStruct.USART_BaudRate = baudrate;				// the baudrate is set to the value we passed into this init function
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;// we want the data frame size to be 8 bits (standard)
	USART_InitStruct.USART_StopBits = USART_StopBits_1;		// we want 1 stop bit (standard)
	USART_InitStruct.USART_Parity = USART_Parity_No;		// we don't want a parity bit (standard)
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // we don't want flow control (standard)
	USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx; // we want to enable the transmitter and the receiver
	USART_Init(USART6, &USART_InitStruct);					// again all the properties are passed to the USART_Init function which takes care of all the bit setting


	/* Here the USART1 receive interrupt is enabled
	 * and the interrupt controller is configured
	 * to jump to the USART1_IRQHandler() function
	 * if the USART1 receive interrupt occurs
	 */
	USART_ITConfig(USART6, USART_IT_RXNE, ENABLE); // enable the USART1 receive interrupt

	NVIC_InitStruct.NVIC_IRQChannel = USART6_IRQn;		 // we want to configure the USART1 interrupts
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;// this sets the priority group of the USART1 interrupts
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;		 // this sets the subpriority inside the group
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;			 // the USART1 interrupts are globally enabled
	NVIC_Init(&NVIC_InitStruct);							 // the properties are passed to the NVIC_Init function which takes care of the low level stuff

	// finally this enables the complete USART1 peripheral
	USART_Cmd(USART6, ENABLE);
}
 
void USART6_IRQHandler(void){

	// check if the USART1 receive interrupt flag was set
	if( USART_GetITStatus(USART6, USART_IT_RXNE) ){
		char t = USART6->DR;
		char data[1];
		data[0] = t;
		USART_puts(USART6, data);
		if (counter < MAX_LENGTH && t != '#') {
			message[counter++] = t;
			return;
		}	
		if (t == '#') {
			if (checkMessage()) {
				sendDataToObserver();
			} else {
				USART_puts(USART6, "\n\r Invalid command! \n\r");
			}
		}
		if (counter > MAX_LENGTH && t != '#') {
			USART_puts(USART6, "\n\r Invalid command! \n\r");
		}
		counter = 0;
	}
}


