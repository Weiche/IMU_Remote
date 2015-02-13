#include "lpc8xx.h"

void SwitchMatrix_Init() 
{ 
    /* Enable SWM clock */
    LPC_SYSCON->SYSAHBCLKCTRL |= (1<<7);

    /* Pin Assign 8 bit Configuration */
    /* SPI0_SCK */
    LPC_SWM->PINASSIGN3 = 0x07ffffffUL; 
    /* SPI0_MOSI */
    /* SPI0_MISO */
    LPC_SWM->PINASSIGN4 = 0xffff0e00UL; 
    /* I2C0_SDA */
    LPC_SWM->PINASSIGN7 = 0x09ffffffUL; 
    /* I2C0_SCL */
    LPC_SWM->PINASSIGN8 = 0xffffff08UL; 

    /* Pin Assign 1 bit Configuration */
    /* SWCLK */
    /* SWDIO */
    /* RESET */
    LPC_SWM->PINENABLE0 = 0xffffffb3UL; 
}
void IOCON_Init() {
    
    /* Enable UART clock */
    LPC_SYSCON->SYSAHBCLKCTRL |= (1<<18);

    /* Pin I/O Configuration */
    /* LPC_IOCON->PIO0_0 = 0x90; */
    /* LPC_IOCON->PIO0_1 = 0x90; */
    /* LPC_IOCON->PIO0_2 = 0x90; */
    /* LPC_IOCON->PIO0_3 = 0x90; */
    /* LPC_IOCON->PIO0_4 = 0x90; */
    /* LPC_IOCON->PIO0_5 = 0x90; */
    /* LPC_IOCON->PIO0_6 = 0x90; */
    /* LPC_IOCON->PIO0_7 = 0x90; */
    /* LPC_IOCON->PIO0_8 = 0x90; */
    /* LPC_IOCON->PIO0_9 = 0x90; */
    /* LPC_IOCON->PIO0_10 = 0x80; */
    /* LPC_IOCON->PIO0_11 = 0x80; */
    /* LPC_IOCON->PIO0_12 = 0x90; */
    LPC_IOCON->PIO0_13 = 0x490;
    LPC_IOCON->PIO0_14 = 0x490;
    /* LPC_IOCON->PIO0_15 = 0x90; */
    /* LPC_IOCON->PIO0_16 = 0x90; */
    /* LPC_IOCON->PIO0_17 = 0x90; */
}

