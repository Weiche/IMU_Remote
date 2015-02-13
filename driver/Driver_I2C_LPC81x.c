#include "lpc8xx.h"
#include "lpc8xx_i2c.h"
#include "Driver_I2C_LPC81x.h"
#if 1
#define I2C_POLLSTAT(notpass,error_handle)	\
timeout = 0xFFFFFFFF;\
while(notpass){timeout--;if(timeout == 0)return -2;}

#else
#define I2C_POLLSTAT(notpass,error_handle)	while(notpass)
#endif

void I2C_Master_Init(uint32_t Bitrate){
	LPC_SYSCON->SYSAHBCLKCTRL |= (1<<5);
	LPC_SYSCON->PRESETCTRL &= ~(1<<6);
	LPC_SYSCON->PRESETCTRL |= (1<<6);

	LPC_I2C->CFG |= CFG_MSTENA;
	
	/* SCL 24M / 12 / 5 = 400kHz */
	LPC_I2C->DIV = ( 24*1000*1000/5/Bitrate ) - 1;
	LPC_I2C->MSTTIME = (0x33) & 0x77;
}
int I2C_Master_Read(const uint8_t addr8bit ,uint8_t *pbuff, uint16_t length ){
	volatile uint32_t timeout;
	if( length == 0 )
		return -1;
	I2C_POLLSTAT( (LPC_I2C->STAT & 0x0E) , {return -3;});
	LPC_I2C->MSTDAT = addr8bit | 0x01;
	LPC_I2C->MSTCTL = CTL_MSTSTART;
	
	while( length != 0 ){
		I2C_POLLSTAT((LPC_I2C->STAT & STAT_MSTPEND) == 0 , return -2);
		*pbuff++ = LPC_I2C->MSTDAT;
		length--;
		if( length == 0 ){
			I2C_POLLSTAT((LPC_I2C->STAT & STAT_MSTPEND) == 0 , return -2);
			LPC_I2C->MSTCTL = CTL_MSTSTOP;
		}else{
			LPC_I2C->MSTCTL = CTL_MSTCONTINUE;
		}
	}
	return 0;
}

int I2C_Master_Write_Register(const uint8_t addr8bit ,const uint8_t reg, uint16_t length , const uint8_t *pbuff){
	uint32_t timeout;
	I2C_POLLSTAT((LPC_I2C->STAT & 0x0E),return -3);
	LPC_I2C->MSTDAT = addr8bit & 0xFE;
	LPC_I2C->MSTCTL = CTL_MSTSTART;
	
	if( reg <= 0xFF ){
		/* write reg byte */
		I2C_POLLSTAT((LPC_I2C->STAT & STAT_MSTPEND) == 0 , return -2);
		LPC_I2C->MSTDAT = reg;
		LPC_I2C->MSTCTL = CTL_MSTCONTINUE;	
	}
	/* send remain bytes */
	while( length != 0 ){
		I2C_POLLSTAT((LPC_I2C->STAT & STAT_MSTPEND) == 0 , return -2);
		LPC_I2C->MSTDAT = *pbuff++;
		length--;
		LPC_I2C->MSTCTL = CTL_MSTCONTINUE;
	}
	I2C_POLLSTAT((LPC_I2C->STAT & STAT_MSTPEND) == 0 , return -2);
	LPC_I2C->MSTCTL = CTL_MSTSTOP;
	I2C_POLLSTAT((LPC_I2C->STAT & STAT_MSTPEND) == 0 , return -2);
	return 0;
}
int I2C_Master_Read_Register(const uint8_t addr8bit,const uint8_t reg , uint16_t length ,uint8_t *pbuff){
	I2C_Master_Write_Register(addr8bit,reg,0,0);
	I2C_Master_Read(addr8bit,pbuff,length);
	return 0;	
}

