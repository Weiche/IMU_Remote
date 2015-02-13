#include "lpc8xx.h"

void sleep(void) {  
    //Normal sleep mode for PCON:
    LPC_PMU->PCON &= ~0x03;
    
    //Normal sleep mode for ARM core:
    SCB->SCR = 0;
    
    //And go to sleep
    __WFI();
} 
void deepsleep(void) {
    //Deep sleep in PCON
    LPC_PMU->PCON &= ~0x03;
    LPC_PMU->PCON |= 0x01;
    
    //If brownout detection and WDT are enabled, keep them enabled during sleep
    LPC_SYSCON->PDSLEEPCFG = LPC_SYSCON->PDRUNCFG;
    
    //After wakeup same stuff as currently enabled:
    LPC_SYSCON->PDAWAKECFG = LPC_SYSCON->PDRUNCFG;
    
    //All interrupts may wake up:
    LPC_SYSCON->STARTERP0 = 0xFF;
    LPC_SYSCON->STARTERP1 = 0xFFFF;
	
    //Deep sleep for ARM core:
    SCB->SCR = 1<<2;
    
    __WFI();
}
