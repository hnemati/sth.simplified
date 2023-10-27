/*
 * arm_common.h
 *
 *  Created on: May 31, 2012
 *      Author: Viktor Do
 */

#ifndef ARM_COMMON_H_
#define ARM_COMMON_H_

#include "arm_common_defs.h"

/* interrupt */
typedef enum {
    IRQ_ENABLE = 0,
    IRQ_DISABLE = 0x80,

    FIQ_ENABLE = 0,
    FIQ_DISABLE = 0x40,

    INTERRUPT_MASK = (IRQ_DISABLE | FIQ_DISABLE)
} interrupt_state;

/* CPU context */
typedef struct context_
{
    uint32_t reg[13];
    uint32_t sp;
    uint32_t lr;
    uint32_t pc;
    uint32_t psr;
} context;


#endif /* ARM_COMMON_H_ */
