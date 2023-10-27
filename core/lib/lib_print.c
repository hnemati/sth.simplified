
#include <stdarg.h>

#include "lib.h"
#include <hw.h>

//Provided here to give guest access without hypercalls for now
void printf_putchar(char c)
{
    stdio_write_char(c);
}
