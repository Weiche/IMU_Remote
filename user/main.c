#include "lpc8xx.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"
#include "inv_mpu_hal_lpc81x.h"
#include "Include/lpc8xx_extend.h"
#include "nrf24l01.h"
#include "xprintf.h"

#define NRF24_MODE 0
extern void SwitchMatrix_Init(void); 
extern void IOCON_Init(void) ;

extern void sleep(void);
extern void deepsleep(void);

extern void MPU6050_Init(void);
extern int MPU6050_DataProcess(void);
extern long quat[4];
extern struct AHRP_Data{
	float pitch;
	float roll;
	float yaw;
	int statu;
}IMU_out;

void MPU6050_Task(void);
void NRF24_CheckStatus(void);
int Radio_ConnectionCheck( void );

volatile uint32_t __System_Tick = 0;
volatile uint8_t MPU6050_Ready = 0;
volatile uint8_t NRF24_TXE = 1;

NRF24_InitTypedef nrf24 = {
	.Address_Length = Address_Length_5Byte,
	.Auto_Retransmit_Count = 15,
	.Auto_Retransmit_Delay = 500 / 250,
	.Channel = 0x0F,
	.CRC_Control = CRC_Control_Enable,
	.CRC_Length = CRC_Length_2Byte,
	.Interrupt_Mask = Interrupt_Mask_RX_DR,
	.Power = Power_On,
	.Mode = Mode_RX,
	.TX_Power = TX_Power_7DB,
	.DataRate = DataRate_2M,
	.AutoAck_Enable_Flag = 0x03
};
#define PACKET_LENGTH 32
const uint8_t RX_Addr[5] = "REMO1";
uint8_t Master_Addr[5] = {1,2,3,4,5};
uint8_t NRF24_TX_Buffer[PACKET_LENGTH] = "";

#define radio_puts(msg)	\
xsprintf((char*)NRF24_TX_Buffer,msg);\
NRF24_SendPacket(&nrf24,NRF24_TX_Buffer,PACKET_LENGTH,0);\
NRF24_CE_Enable();\
while(( NRF24_Read_Reg(STATUS)&STATUS_TX_DS) == 0);\
NRF24_CE_Disable()

#define WDT_FEED()	\
LPC_WWDT->FEED = 0xAA;\
LPC_WWDT->FEED = 0x55


int main(void){
	int packet_num = 0;	
	
	/* Systick initialize */
	SysTick_Config( SystemCoreClock / 1000 - 1 );
	NVIC_EnableIRQ(SysTick_IRQn);
	
	
	/* periphral initialize */
	I2C_Master_Init(300000);
	SYSCON_PeriInit(WKT);
	SYSCON_PeriEnable(WWDT);
	LPC_SYSCON->PDRUNCFG &= ~(1<<6);
	LPC_SYSCON->WDTOSCCTRL = (0x5 << 5);/* 2.1MHz / 2 */
	
	/* Pin function setting */
	SwitchMatrix_Init();
	IOCON_Init();
	
	LPC_WWDT->TC = (1000)*(2000) - 1;
	LPC_WWDT->MOD|= 0x03;/* Reset mode */
	WDT_FEED();	
	
	/* watch dog (only) will start after feed once */
	while( NRF24_HAL_Test() );
	NRF24_Init(&nrf24);	
	
	NRF24_SetRX_Length(0, PACKET_LENGTH);
	NRF24_SetRX_Length(1, PACKET_LENGTH);
	
	NRF24_Set_RX_Addr(RX_Addr,1);
	NRF24_Set_RX_Addr(Master_Addr,0);
	NRF24_Set_TX_Addr(Master_Addr);
	
	PinInt_Init( 1, 16, PININT_Mode_Falling );	
	PinInt_Init(0,15, PININT_Mode_Falling);
	
	
	radio_puts("\nConnecting...OK\n");
	radio_puts("IMU Initializing..\n");	
	
	MPU6050_Init();
	radio_puts("OK\nTest IMU...");	

	while(1){
		WDT_FEED();
		NRF24_CheckStatus();
		deepsleep();
		
		if( MPU6050_Ready ){
			MPU6050_Task();
			if( ( NRF24_Read_Reg(FIFO_STATUS)&(1<<4) )){//if TX fifo empty
				if( IMU_out.statu == 0 ){
					xsprintf((char*)NRF24_TX_Buffer,"%d:P%d,R%d,Y%d\n",packet_num++,(int)IMU_out.pitch,(int)IMU_out.roll, (int)IMU_out.yaw);
				}else{
					xsprintf((char*)NRF24_TX_Buffer,"%d:IMU_ERROR!\n",packet_num++);
				}
				NRF24_SendPacket(&nrf24,NRF24_TX_Buffer,PACKET_LENGTH,1);
				NRF24_CE_Enable();
				while(( NRF24_Read_Reg(STATUS)&STATUS_TX_DS) == 0);
				NRF24_CE_Disable();				
			}
		}	
		
		
	}
}

void MPU6050_Task(void){
	int ret;
	ret = MPU6050_DataProcess();
	
	if( ret ){
		/*when read failed*/
	}
	MPU6050_Ready = 0;
}



void NRF24_CheckStatus(void){
	static uint8_t status;
	static uint8_t fifo_status;
	
	fifo_status = NRF24_Read_Reg(FIFO_STATUS);
	status = NRF24_Read_Reg(STATUS);
	/* clear max retransmittion flag */
	if( status & STATUS_MAX_RT ){
			NRF24_Write_Reg(STATUS,STATUS_MAX_RT);
			NRF24_CE_Enable();
	}
	/* clear transfer complete flag */
	if(status & STATUS_TX_DS){
		NRF24_Write_Reg(STATUS,STATUS_TX_DS);
	}
	
	
	if( (fifo_status &(1<<5)) == 0){ //if TX fifo not full
		NRF24_TXE = 1;		
	}
	/* if TX empty, disable the CE pin */
	if(fifo_status & (1<<4)){
		NRF24_CE_Disable();		
	}else{
		NRF24_CE_Enable();
	}
}
/* Check parent */
int Radio_ConnectionCheck( void ){
	int ret;
	uint8_t byte_status;
	
	xsprintf((char*)NRF24_TX_Buffer,"CHECK\n");
	/* Send a packet request ack */
	NRF24_SendPacket(&nrf24,NRF24_TX_Buffer,PACKET_LENGTH,0);
	NRF24_CE_Enable();
	while(1){
		byte_status = NRF24_Read_Reg(STATUS);
		if( byte_status & STATUS_TX_DS ){			
			ret = 0;
			break;
		}else if ( byte_status & STATUS_MAX_RT ){
			ret = 1;
			break;
		}
	}
	NRF24_CE_Disable();
	NRF24_Write_Reg(STATUS,STATUS_TX_DS|STATUS_MAX_RT);
	return ret;
}
/* Interrupt handlers */
void PININT0_IRQHandler (void){
	LPC_PIN_INT->IST |= (1<<0);
	MPU6050_Ready = 1;
}

void PININT1_IRQHandler(void) {
	LPC_PIN_INT->IST |= (1<<1);
}

void SysTick_Handler(void){
	__System_Tick++;
}
