#include <stm32f4xx.h>
#include <misc.h>			 // I recommend you have a look at these in the ST firmware folder
#include <stm32f4xx_usart.h> // under Libraries/STM32F4xx_StdPeriph_Driver/inc and src
#include <string.h>
#define MAX_STRLEN 12 // this is the maximum string length of our string in characters

char received_string[MAX_STRLEN+1]; // this will hold the recieved string

volatile int cnt = 0;

void init_USART1(int baudrate);
void init_USART6(int baudrate);
void init_adc(void);
void USART_puts(USART_TypeDef* USARTx, char* data);

int compare_exit() {
		received_string[cnt + 1] = '\0';
		if (!strcmp(received_string, "exit")) {
			return 1;
		}
		return 0;
}

void Delay(__IO uint32_t nCount) {
  while(nCount--) {
  }
}

/**
* Glavni dio programa. Dojava uspjesnog povezivanja i cekanje prekida.
*/
int main(void) {
  init_USART1(29000); // inicijalizacija USART1  @9600 baud
	init_USART6(29000); // inicijalizacija USART6  @9600 baud
	init_adc(); // inicijalizacija ADC i temoperaturnog senzora
	
	USART_puts(USART6, "Dobro dosli u STM32F4 wireless shell v 1.0\r\n");
	USART_puts(USART1, "***Cekanje podataka sa senzora***\r\n");
	while (1){
			//cekanje prekida
  }
}

// this is the interrupt request handler (IRQ) for ALL USART1 interrupts
void USART1_IRQHandler2(void){

	// check if the USART1 receive interrupt flag was set
	if( USART_GetITStatus(USART1, USART_IT_RXNE) ){

		//static uint8_t cnt = 0; // this counter is used to determine the string length
		char t = USART1->DR; // the character from the USART1 data register is saved in t

		/* check if the received character is not the LF character (used to determine end of string)
		 * or the if the maximum string length has been been reached
		 */
		if( (t != 'n') && (cnt < MAX_STRLEN) ){
			received_string[cnt] = t;
			cnt++;
		}
		else{ // otherwise reset the character counter and print the received string
			cnt = 0;
			USART_puts(USART1, received_string);
		}
	}
}

void USART1_IRQHandler(void) {
		// check if the USART1 receive interrupt flag was set
	if( USART_GetITStatus(USART1, USART_IT_RXNE) ){

		static uint8_t cnt = 0; // this counter is used to determine the string length
		char t = USART1->DR; // the character from the USART1 data register is saved in t

		/* check if the received character is not the LF character (used to determine end of string)
		 * or the if the maximum string length has been been reached
		 */
		char data[1];
		data[0] = t;
		USART_puts(USART1, data);
		if( (t != '#') && (cnt < MAX_STRLEN) ){
			received_string[cnt] = t;
			cnt++;
			return;
		}
		
		if ( t == '#' ) {
			if (compare_exit()) {
				USART_puts(USART1, "\n\rPokusali ste izaci, ali odavde izlaza nema!!!\n\r");
			} else {
				USART_puts(USART1, "\n\rPogresna naredba!\n\r");
			}				
			cnt = 0;
			return;			
		}
			cnt = 0;
		  USART_puts(USART1, "\n\rNepoznato: ");
			USART_puts(USART1, received_string);
			USART_puts (USART1, "\r\n");
		}
	}


	


