#ifndef __INV_MPU_HAL_LPC81x_H
#define __INV_MPU_HAL_LPC81x_H
/* The following functions must be defined for this platform:
 * i2c_write(unsigned char slave_addr, unsigned char reg_addr,
 *      unsigned char length, unsigned char const *data)
 * i2c_read(unsigned char slave_addr, unsigned char reg_addr,
 *      unsigned char length, unsigned char *data)
 * delay_ms(unsigned long num_ms)
 * get_ms(unsigned long *count)
 * reg_int_cb(void (*cb)(void), unsigned char port, unsigned char pin)
 * labs(long x) int abs
 * fabsf(float x) float abs
 * min(int a, int b)
 * __no_operation()   NOP
 */
 
#include "driver/Driver_I2C_LPC81x.h"
#include "inv_mpu.h"
/* i2c communication interface */	
#define i2c_write(addr, reg, len, pointer)   I2C_Master_Write_Register((addr<<1), reg, len, pointer)
#define i2c_read(addr, reg, len, pointer)    I2C_Master_Read_Register((addr<<1), reg, len, pointer)
/* TODO add delay_ms and get_ms */
extern volatile uint32_t __System_Tick;
static void delay_ms(unsigned long num_ms){ 
	uint32_t target = num_ms + __System_Tick;
	while( __System_Tick < target ){
		__nop();
	}
}
#define get_ms(pcount) *pcount = __System_Tick
	
/* regist interrupt callback function */
static inline int reg_int_cb(struct int_param_s *int_param)
{    
	/* set an interrupt ,when int pin goes low to call and cb in struct int_param_s  */
    return 0;
}
/* log error and infomation (optional)*/
#define log_i(...)     do {} while (0)
#define log_e(...)     do {} while (0)
/* LPC812 is a 32-bit processor, so abs and labs are equivalent. */
#define labs		abs
//#define fabs(x)		(((x)>0)?(x):-(x))
#define min(a,b)	((a<b)?a:b)
#define __no_operation()	__nop()
#endif
