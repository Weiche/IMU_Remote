/**********************************************************************
* $Id$		lpc8xx_i2c.h		2011-06-02
*//**
* @file		lpc8xx_i2c.h
* @brief	Contains all macro definitions and function prototypes
* 			support for I2C firmware library on lpc18ax
* @version	1.0
* @date		02. June. 2011
* @author	NXP MCU SW Application Team
*
* Copyright(C) 2011, NXP Semiconductor
* All rights reserved.
*
***********************************************************************
* Software that is described herein is for illustrative purposes only
* which provides customers with programming information regarding the
* products. This software is supplied "AS IS" without any warranties.
* NXP Semiconductors assumes no responsibility or liability for the
* use of the software, conveys no license or title under any patent,
* copyright, or mask work right to the product. NXP Semiconductors
* reserves the right to make changes in the software without
* notification. NXP Semiconductors also make no representation or
* warranty that such application will be suitable for the specified
* use without further testing or modification.
* Permission to use, copy, modify, and distribute this software and its
* documentation is hereby granted, under NXP Semiconductors�
* relevant copyright in the software, without fee, provided that it
* is used in conjunction with NXP Semiconductors microcontrollers.  This
* copyright, permission, and disclaimer notice must appear in all copies of
* this code.
**********************************************************************/

/* Peripheral group ----------------------------------------------------------- */
/** @defgroup I2C
 * @ingroup LPC800CMSIS_FwLib_Drivers
 * @{
 */

#ifndef lpc8xx_I2C_H_
#define lpc8xx_I2C_H_

/* Includes ------------------------------------------------------------------- */
#include "LPC8xx.h"

#ifdef __cplusplus
extern "C"
{
#endif
/* Public Types --------------------------------------------------------------- */
/** @defgroup I2C_Public_Types I2C Public Types
 * @{
 */
typedef struct {
	LPC_I2C_TypeDef *I2Cx;
	uint8_t		addr[4];
	uint8_t		cfg;
	void (*slv_cb[4]) (void*);
}i2cdev_slv;
typedef struct{
	enum{
	ADDR,
	TX,
	RX
	}state;
	uint8_t data;
}I2C_CallbackArgs;

#define I2C_INTERRUPT			1		/* For TXRDY and RXRDY test. */
#define I2C_MONITOR_MODE	0
#define ADDR_QUAL_ENABLE	0
#define TIMEOUT_ENABLED		0
#define SLAVE_POLLING_TEST	0		/* if I2C_INTERRUPT is 1, SLAVE TX/RX will be handled inside 
																ISR automatically. This flag will not be needed. */

/* I2C read and write buffer size */
#define I2C_BUFSIZE    	0x10
#define I2C_MONBUFSIZE	0x40

#define TIMEOUT_VALUE		0x10



#define RD_BIT              0x01

/* When the I2C is a slave, SLAVE_ADDR is the slave address. */ 
#define SLAVE_ADDR			0x80

  /* For master mode plus, if desired I2C clock is 1MHz (SCL high time + SCL low time). 
	If CCLK is 36MHz, MasterSclLow and MasterSclHigh are 0s, 
	SCL high time = (ClkDiv+1) * (MstSclHigh + 2 )
	SCL low time = (ClkDiv+1) * (MstSclLow + 2 )
	Pre-divider should be 36000000/(1000000*4)-1 = 9-1 = 8. 
	If fast mode, e.g. communicating with a temp sensor, Max I2C clock is set to 400KHz.
	Pre-divider should be 36000000/(400000*4)-1 = 22.5-1 = 22.
	If standard mode, e.g. communicating with a temp sensor, Max I2C clock is set to 100KHz.
	Pre-divider should be 36000000/(100000*4)-1 = 90-1 = 89. */
#define I2C_FMODE_PLUS_PRE_DIV	(9-1)			/* 1MHz */
#define I2C_FMODE_PRE_DIV				(23-1)		/* 400KHz */
#define I2C_SMODE_PRE_DIV				(90-1)		/* 100KHz */

#define CFG_MSTENA				(1 << 0)
#define CFG_SLVENA				(1 << 1)
#define CFG_MONENA				(1 << 2)
#define CFG_TIMEOUTENA		(1 << 3)
#define CFG_MONCLKSTR			(1 << 4)

#define CTL_MSTCONTINUE		(1 << 0)
#define CTL_MSTSTART			(1 << 1)
#define CTL_MSTSTOP 			(1 << 2)

#define CTL_SLVCONTINUE		(1 << 0)
#define CTL_SLVNACK				(1 << 1)

#define TIM_MSTSCLLOW(d)	((d) << 0)
#define TIM_MSTSCLHIGH(d)	((d) << 4)

#define STAT_MSTPEND  		(1 << 0)
#define MASTER_STATE_MASK	(0x7<<1)
#define STAT_MSTIDLE	 		(0x0 << 1)
#define STAT_MSTRX	 			(0x1 << 1)
#define STAT_MSTTX	 			(0x2 << 1)
#define STAT_MSTNACKADDR	(0x3 << 1)
#define STAT_MSTNACKTX		(0x4 << 1)
#define STAT_MSTARBLOSS		(1 << 4)
#define STAT_MSTSSERR	 		(1 << 6)
#define STAT_MST_ERROR_MASK	(MSTNACKADDR|STAT_MSTNACKTX|STAT_MSTARBLOSS|STAT_MSTSSERR)

#define STAT_SLVPEND 			(1 << 8)
#define SLAVE_STATE_MASK	(0x3<<9)
#define STAT_SLVADDR			(0x0 << 9)
#define STAT_SLVRX  	 		(0x1 << 9)
#define STAT_SLVTX  	 		(0x2 << 9)
#define STAT_SLVNOTSTR		(1 << 11)
#define STAT_SLVSEL		 		(1 << 14)
#define STAT_SLVDESEL			(1 << 15)

#define STAT_MONRDY				(1 << 16)
#define STAT_MONOVERRUN 	(1 << 17)
#define STAT_MONACTIVE		(1 << 18)
#define STAT_MONIDLE			(1 << 19)

#define STAT_EVTIMEOUT		(1 << 24)
#define STAT_SCLTIMEOUT		(1 << 25)

void I2C_IRQHandler(void);
int I2C_Open( i2cdev_slv *dev );
int I2C_Listen( i2cdev_slv *dev , uint8_t addr_num, uint8_t addr ,void cb(void *));
#define I2C_SLV_RESET(I2Cx)	I2Cx->CFG &=~ CFG_SLVENA;I2Cx->CFG |= CFG_SLVENA
/**
 * @}
 */


#ifdef __cplusplus
}
#endif


#endif /* lpc8xx_I2C_H_ */

/**
 * @}
 */
