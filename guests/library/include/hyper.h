#ifndef _HYPER_H_
#define _HYPER_H_

#include "types.h"
#include "arm_common.h"

//-------------
// Constants
//-------------


// guest modes
#define MODE_NONE ((uint32_t)-1)

//Hypercalls
#define HYPERCALL_MESSAGE            1
#define HYPERCALL_RELEASE            0

#define HYPERCALL_REG_VM			0

#define HYPERCALL_SETMODECONTEXT    10
#define HYPERCALL_GETMODECONTEXT    11

#define HYPERCALL_END_INTERRUPT     30



#define STR(x) #x
#define HYPERCALL_NUM(n) "#"STR(n)

/* Hypercall numbers as strings for inline asm */
#define HYPERCALL_REG_VM_NUM            HYPERCALL_NUM(HYPERCALL_REG_VM)

#define HYPERCALL_SETMODECONTEXT_NUM    HYPERCALL_NUM(HYPERCALL_SETMODECONTEXT)
#define HYPERCALL_GETMODECONTEXT_NUM    HYPERCALL_NUM(HYPERCALL_GETMODECONTEXT)

#define HYPERCALL_END_INTERRUPT_NUM     HYPERCALL_NUM(HYPERCALL_END_INTERRUPT)


//-------------
// ASM macros
//-------------

// Dont' know what registers the first call will use,
// so can't make this one clean (well, there probably is a way...)
#define ISSUE_HYPERCALL_ARGS(num, pArgs) \
	asm volatile ("mov R1, %0" :: "r"((pArgs))); \
	asm volatile ( \
		"SWI " HYPERCALL_NUM((num)) "         \n\t" \
	);

#define ISSUE_HYPERCALL(num) \
	asm volatile ( \
		"SWI " HYPERCALL_NUM((num)) "         \n\t" \
	);

// set user mode SP to a variable value
// (must be a variable, since we take address)
#define SET_USER_SP(value) \
	asm ("LDMIA %0, {SP}^ \n\t" :: "r"(&(value)));


// -------------
//   structures
// -------------


typedef void(*interrupt_handler_fn)(void);

/*
 * Used by guest to register VM
 */
typedef struct reg_vm_
{
	uint32_t guest_tick_handler;
} reg_vm;

typedef struct set_mode_context_args_
{
	uint32_t mode;
	const context* ctx;
} set_mode_context_args;

typedef struct get_mode_context_args_
{
	uint32_t mode;
	context* ctx;
} get_mode_context_args;

#endif
