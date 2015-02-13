#ifndef LPC8xx_EXT_H
#define LPC8xx_EXT_H
#ifndef RELEASE
	#define ASSERT(a)	if(!a){while(1);}
#else
	#define ASSERT(a)
#endif
/* SYSCON AHB CTRL RESISTER */
#define	AHBCTRL_SYS			(1<<0)
#define	AHBCTRL_ROM			(1<<1)
#define	AHBCTRL_RAM			(1<<2)
#define	AHBCTRL_FLASHREG	(1<<3)
#define	AHBCTRL_FLASH		(1<<4)
#define	AHBCTRL_I2C			(1<<5)
#define	AHBCTRL_GPIO		(1<<6)
#define	AHBCTRL_SWM			(1<<7)
#define	AHBCTRL_SCT			(1<<8)
#define	AHBCTRL_WKT			(1<<9)
#define	AHBCTRL_MRT			(1<<10)
#define	AHBCTRL_SPI0		(1<<11)
#define	AHBCTRL_SPI1		(1<<12)
#define	AHBCTRL_CRC			(1<<13)
#define	AHBCTRL_UART0		(1<<14)
#define	AHBCTRL_UART1		(1<<15)
#define	AHBCTRL_UART2		(1<<16)
#define	AHBCTRL_WWDT		(1<<17)
#define	AHBCTRL_IOCON		(1<<18)
/* Presetctrl */


#define	PRESETCTRL_SPI0		(1<<0)

#define	PRESETCTRL_SPI1		(1<<1)

#define	PRESETCTRL_UARTFRG		(1<<2)

#define	PRESETCTRL_UART0		(1<<3)

#define	PRESETCTRL_UART1		(1<<4)

#define	PRESETCTRL_UART2		(1<<5)

#define	PRESETCTRL_I2C		(1<<6)

#define	PRESETCTRL_MRT		(1<<7)

#define	PRESETCTRL_SCT		(1<<8)

#define	PRESETCTRL_WKT		(1<<9)

#define	PRESETCTRL_GPIO		(1<<10)

#define	PRESETCTRL_FLASH		(1<<11)

#define	PRESETCTRL_ACMP		(1<<12)

#define SYSCON_PeriEnable(a)	\
	LPC_SYSCON->SYSAHBCLKCTRL |= AHBCTRL_##a;

#define SYSCON_PeriReset(a)		\
	LPC_SYSCON->PRESETCTRL &= ~(PRESETCTRL_##a);\
	LPC_SYSCON->PRESETCTRL |= (PRESETCTRL_##a);

#define SYSCON_PeripheralInit(a)		SYSCON_PeriEnable(a);SYSCON_PeriReset(a);
#define SYSCON_PeriInit(a)	SYSCON_PeripheralInit(a)
/* pin interrupt
TODO put into lpc8xx_pinint.h */
typedef enum{
	PININT_Mode_Falling,
	PININT_Mode_Rising
}PININT_Mode;
void PinInt_Init(int Pin_int, int Pin, PININT_Mode mode);
/* End pin interrupt */
#endif
