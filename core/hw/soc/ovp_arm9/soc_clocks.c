#include "hw.h"

void soc_clocks_init()
{
    powersaving_registers * power = (powersaving_registers *) POWERSAVING_BASE;
    
    /* AND POWER FOR ALL: clock enable for all peripherals */
    power->pcer = -1;
}
