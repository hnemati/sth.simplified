
    .global cpu_interrupt_set
    .global cpu_interrupt_user_set

    .code 32
    .align  0

#include "arm_common_defs.h"


cpu_interrupt_set:
    mrs r1, CPSR
    and r2, r1, #ARM_IRQ_MASK | ARM_FIQ_MASK
    bic r1, #ARM_IRQ_MASK | ARM_FIQ_MASK
    orr r1, r0
#    mov r0, r2
	mov r0, r1
    msr CPSR_c, r1
    bx lr
    

cpu_interrupt_user_set:
    mrs r1, SPSR
    and r2, r1, #ARM_IRQ_MASK | ARM_FIQ_MASK
    bic r1, #ARM_IRQ_MASK | ARM_FIQ_MASK
    orr r1, r0
#    mov r0, r2
	mov r0, r1
    msr SPSR_c, r1
    bx lr

