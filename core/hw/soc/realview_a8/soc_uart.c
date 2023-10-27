#include "hw.h"

#include "soc.h"
#include "soc_private.h"


#define BASE_UART0 0x10009000
#define BASE_UART1 0x1000A000
#define BASE_UART2 0x1000B000
#define BASE_UART3 0x1000C000


#define PL011_DR   ( 0x00 / 4)
#define PL011_FR   ( 0x18 / 4)
#define PL011_IBRD ( 0x24 / 4)
#define PL011_FBRD ( 0x28 / 4)
#define PL011_CR   ( 0x30 / 4)
#define PL011_FR_TXFF 5


void stdio_write_char(int c)
{
    volatile uint32_t *base = (uint32_t *) BASE_UART0;

    for(;;) {
        while( (base[PL011_FR] & (1UL << PL011_FR_TXFF)) )
            ; /* wait for queue space */

        base[PL011_DR] = c;

        if(c != '\n')
            break;
        c = '\r';
    }
}

void soc_uart_init()
{
    volatile uint32_t *base = (uint32_t *) BASE_UART0;
    base[PL011_CR] = 0x301;
}
