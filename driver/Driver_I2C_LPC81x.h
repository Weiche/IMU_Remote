/*
 * Driver_I2C_LPC8xx.h
 *
 *  Created on: 2014?7?4?
 *      Author: Administrator
 */

#ifndef DRIVER_I2C_LPC8XX_H_
#define DRIVER_I2C_LPC8XX_H_

#include "lpc8xx.h"
#include "lpc8xx_i2c.h"
void I2C_Master_Init(uint32_t Bitrate);
int I2C_Master_Read(const uint8_t addr8bit ,uint8_t *pbuff, uint16_t length );

int I2C_Master_Write_Register(const uint8_t addr8bit ,const uint8_t reg, uint16_t length , const uint8_t *pbuff);
int I2C_Master_Read_Register(const uint8_t addr8bit,const uint8_t reg , uint16_t length ,uint8_t *pbuff);

#endif /* DRIVER_I2C_LPC8XX_H_ */
