#include <types.h>

memory_layout_entry memory_padr_layout[] = 
{
    {ADR_TO_PAGE(0x00000000), ADR_TO_PAGE(0x000fffff), MLT_HYPER_RAM, MLF_READABLE | MLF_WRITEABLE }, // hypervisor ram
    {ADR_TO_PAGE(0x00100000), ADR_TO_PAGE(0x002fffff), MLT_USER1_RAM, MLF_READABLE | MLF_WRITEABLE }, // user ram
    {ADR_TO_PAGE(0x00400000), ADR_TO_PAGE(0x002fffff), MLT_USER2_RAM, MLF_READABLE | MLF_WRITEABLE }, // user ram
    {ADR_TO_PAGE(0xfff00000), ADR_TO_PAGE(0xfffe3fff), MLT_HYPER_RAM, MLF_READABLE | MLF_WRITEABLE}, // special function, USART0, TC    
    {ADR_TO_PAGE(0xffff0000), ADR_TO_PAGE(0xffffbfff), MLT_HYPER_RAM, MLF_READABLE | MLF_WRITEABLE}, // WD, power save, PIO   
    {ADR_TO_PAGE(0xffffF000), ADR_TO_PAGE(0xfffffFFF), MLT_HYPER_RAM, MLF_READABLE | MLF_WRITEABLE | MLF_LAST}, // AIC
};
