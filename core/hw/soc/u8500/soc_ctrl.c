

#include <hw.h>

#include "soc_defs.h"

/*
REG 80157018 => 00000300
REG 8000F000 => 00000000
REG 8000F010 => 000001FF
REG 8012F000 => 00000000
REG 8012F010 => 00000FFF
REG 80007030 => 00000101
REG 80007004 => 00000000
REG 80007024 => 00000014
REG 80007028 => 00000035
REG 8000702C => 00000070
REG 8012E020 => 60000000
REG 8012E024 => 60000000
REG 80007034 => 0000001B
REG 80007044 => 00000000
REG 80007038 => 00000780
 */

void soc_ctrl_init()
{
    reg *prcmu = (reg *) PRCMU_BASE;
    reg *prcc3 = (reg *) PRCC_CLK_RST3_BASE;
    reg *prcc1 = (reg *) PRCC_CLK_RST1_BASE;
    reg *gpio0 = (reg *) GPIO0_BASE;
          
    /* UART2 clock 1... */
    prcmu[PRCMU_UARTCLK_MGT] = PRCM_CLK38 | PRCM_CLKEN;
    
    /* UART2 clock 2... */
    prcc3[PRCC_PCKEN] = prcc3[PRCC_PCKSR] | 0x40; /* CLK_P3_UART2 */
    prcc3[PRCC_KCKEN] = prcc3[PRCC_PCKSR] | 0x40; /* CLK_P3_UART2 */

    /* GPIO for UART2 */
    prcc1[PRCC_PCKEN] = prcc1[PRCC_PCKSR] | 0x200; /* CLK_P1_GPIOCTRL */
    gpio0[GPIO_AFSLA] |= 0x60000000; /* GPIO alternate function */
    gpio0[GPIO_AFSLB] |= 0x60000000; /* GPIO alternate function */
          
}

