#ifndef _ARM_COMMON_DEFS_H_
#define _ARM_COMMON_DEFS_H_

/*
 * This header is used by assembler files, keep it clean!
 */

// ARM CPU MODES
#define ARM_MODE_MASK       0x1f
#define ARM_MODE_USER       0x10
#define ARM_MODE_FIQ        0x11
#define ARM_MODE_IRQ        0x12
#define ARM_MODE_SUPERVISOR 0x13
#define ARM_MODE_ABORT      0x17
#define ARM_MODE_UNDEFINED  0x1b
#define ARM_MODE_SYSTEM     0x1f

#define ARM_IRQ_MASK 0x80
#define ARM_FIQ_MASK 0x40
#define ARM_INTERRUPT_MASK ( ARM_IRQ_MASK | ARM_FIQ_MASK)

/* COP */
#define COP_SYSTEM "p15, 0"

#define COP_WRITE(cop, func, reg) \
	asm volatile("mcr " cop ", %0," func :: "r" (reg))

#define COP_READ(cop, func, reg) \
	asm volatile("mrc " cop ", %0, " func : "=r" (reg))


#endif /* _ARM_COMMON_DEFS_H_ */
