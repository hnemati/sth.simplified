/* This is the entry point of the SICS Thin Hypervisor */
/*
 * boot code from ARMv5
 *
 * NOTE: this file only contains the reset code, the
 * exception table and other exception handlers are under SoC
 */

    .global impl_reset
    .extern init_

    .code 32
    .align 0

#include "arm_common_defs.h"

impl_reset:
    /* Start in supervisor mode, disable interrupts */
    msr CPSR_c, #ARM_MODE_SUPERVISOR | ARM_INTERRUPT_MASK
    
    /* clean BSS */
    mov r0, #0
    ldr	r1,=__bss_start__
    ldr	r2,=__bss_end__
bss_loop:
    str r0, [r1], #4
    cmp r1, r2
    blo bss_loop

    /* set up stacks for all CPU modes*/
	ldr r0, = __hyperstack_top__

    msr CPSR_c, #ARM_MODE_IRQ | ARM_INTERRUPT_MASK
    mov sp, r0
    sub r0, r0, #0x1000

    msr CPSR_c, #ARM_MODE_FIQ | ARM_INTERRUPT_MASK
    mov sp, r0
    sub r0, r0, #0x1000

    msr CPSR_c, #ARM_MODE_ABORT | ARM_INTERRUPT_MASK
    mov sp, r0
    sub r0, r0, #0x1000

    msr CPSR_c, #ARM_MODE_UNDEFINED | ARM_INTERRUPT_MASK
    mov sp, r0
    sub r0, r0, #0x1000

    msr CPSR_c, #ARM_MODE_SYSTEM | ARM_INTERRUPT_MASK
    mov sp, r0
    sub r0, r0, #0x1000

    msr CPSR_c, #ARM_MODE_SUPERVISOR | ARM_INTERRUPT_MASK
    mov sp, r0
    sub r0, r0, #0x1000

	bl init_

	/* Should not be reached */
_hang:
	bl .
