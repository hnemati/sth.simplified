__page_size__ = 0x1000; /*4KB page size*/
__ram_start__ = 0x100000;
__ram_end__   = 0x88000000;

MEMORY
{
    RAM (wx) : org = 0x100000, l = 12M
}

ENTRY(_start)

SECTIONS
{
    . = __ram_start__;

	.startup : {
		__startup_start__ = .;
        *(.startup)
        __startup_end__ = .;
	} > RAM
   
    .text : {
        __code_start__ = .;
        *.o (.text)
        __code_end__ = .;
    } > RAM

    .bss ALIGN(__page_size__) : {
        __bss_start__ = .;
        *.o (.bss)
        __bss_end__ = .;
    } > RAM

    .data : {
        __data_start__ = .;
        *.o (.data)
        __data_end__ = .;
    } > RAM

    . = ALIGN(__page_size__);
    __heap_start__ = .;
}
MEMORY
{
    handlersc (wx) : ORIGIN = 0x200000, LENGTH = 1M
}
ENTRY(_start)
SECTIONS { HANDLERSC : { *(.text) } > handlersc }
