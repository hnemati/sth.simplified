#include <types.h>

memory_layout_entry memory_padr_layout[] =
{
    {ADR_TO_PAGE(0x00000000), ADR_TO_PAGE(0x000fffff), MLT_HYPER_RAM, MLF_READABLE | MLF_WRITEABLE }, // hypervisor ram
    {ADR_TO_PAGE(0x00100000), ADR_TO_PAGE(0x002fffff), MLT_USER1_RAM, MLF_READABLE | MLF_WRITEABLE }, // user ram
    {ADR_TO_PAGE(0x00400000), ADR_TO_PAGE(0x006fffff), MLT_USER2_RAM, MLF_READABLE | MLF_WRITEABLE }, // user ram
    {ADR_TO_PAGE(0x10000000), ADR_TO_PAGE(0x1fffffff), MLT_IO_REG, MLF_LAST | MLF_READABLE | MLF_WRITEABLE }, // speripherals
};
