
    .global _start
    .extern _main
    .extern lib_init
    .extern lib_atexit    

    .code 32
    .section .startup, "ax"
    .balign 4
    
    
_start:
start:

  
    /* clean BSS */
    mov r0, #0
    ldr	r1,=__bss_start__
    ldr	r2,=__bss_end__
bss_loop:
    str r0, [r1], #4
    cmp r1, r2
    blo bss_loop
    

    /* set stack pointer at the end of the MB */

// 	ldr r1, = 0x80200000
    ldr r1, =0x00200000
    sub sp, r1, #4

    bl _main

    /* all done */
    b . /* TODO: a real exit */        
