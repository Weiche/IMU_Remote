#include "lpc8xx.h"
#include "lpc8xx_uart.h"
#if (NRF24_MODE == 1)

#define BUFFER_SIZE	1024 * 2
char Vitual_UART [BUFFER_SIZE];
uint32_t Vitual_UART_Index = 0;
uint32_t Vitual_UART_ReadIndex = 0;
uint32_t Vitual_UART_Remain = BUFFER_SIZE;
uint32_t Vitual_UART_Overflow = 0;
void UART0_Putchar( char c ){
	#if 0
	while(!(LPC_USART0->STAT & TXRDY ));
	LPC_USART0->TXDATA  = c;	
	#else
	if( Vitual_UART_Index + 1 == Vitual_UART_ReadIndex){Vitual_UART_Overflow ++; return;}
	
	Vitual_UART[Vitual_UART_Index] = c;
	Vitual_UART_Index++;
	Vitual_UART_Index &= BUFFER_SIZE - 1;
	Vitual_UART_Remain--;
	#endif
}

void NRF24TX_Task(void){
	uint8_t i;
	while( Vitual_UART_ReadIndex != Vitual_UART_Index ){
		if((NRF24_Read_Reg(FIFO_STATUS)	&(1<<5)) == 0){//if TX fifo not full
			i = 0;
			while( (Vitual_UART_ReadIndex != Vitual_UART_Index) && i < PACKET_LENGTH){
				NRF24_TX_Buffer[i] = Vitual_UART[Vitual_UART_ReadIndex];
				i++;				
				Vitual_UART_ReadIndex++;
				Vitual_UART_ReadIndex &= BUFFER_SIZE - 1;
				Vitual_UART_Remain++;
			}
			while( i < PACKET_LENGTH ){
				NRF24_TX_Buffer[i++] = 0;
			}
			NRF24_Send(&nrf24,NRF24_TX_Buffer,PACKET_LENGTH);
			NRF24_CE_Enable();					
		}else{
			NRF24_TXE = 0;
			break;
		}	

	}
		
}
#endif

void UART0_Init(void){
	LPC_SYSCON->UARTCLKDIV = 1;     /* divided by 1 */
	NVIC_DisableIRQ(UART0_IRQn);
	/* Enable UART clock */
	LPC_SYSCON->SYSAHBCLKCTRL |= (1<<14);
	/* Peripheral reset control to UART, a "1" bring it out of reset. */
	LPC_SYSCON->PRESETCTRL &= ~(0x1<<3);
	LPC_SYSCON->PRESETCTRL |= (0x1<<3);
	
	LPC_USART0->CFG = DATA_LENG_8|PARITY_NONE|STOP_BIT_1; /* 8 bits, no Parity, 1 Stop bit */
	LPC_USART0->BRG = SystemCoreClock / 16 / 115200 - 1;
	
	LPC_USART0->CFG |= UART_EN;
}

