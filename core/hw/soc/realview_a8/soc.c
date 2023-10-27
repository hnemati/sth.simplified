#include "hw.h"

#include "soc.h"
#include "soc_private.h"

void soc_ctrl_init()
{
}


void soc_init()
{
    soc_ctrl_init();
    soc_uart_init();
    soc_irq_init();
    soc_timer_init();
}
