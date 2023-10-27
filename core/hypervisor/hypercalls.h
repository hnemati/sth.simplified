
#ifndef HYPERCALLS_H_
#define HYPERCALLS_H_

#include "hyper.h"

//Used by hypervisor to change guest mode for guest
extern void change_guest_mode (uint32_t mode);

/*
 * Hypercalls
 */
extern void hypercall_reg_vm(reg_vm *args);
extern void hypercall_end_interrupt(void);
extern void hypercall_set_mode_context (set_mode_context_args* args);
extern void hypercall_get_mode_context (get_mode_context_args* args);

/*
 * Error handling code
 */

extern void hypercall_num_error (uint32_t hypercall_num);
extern void hyper_panic(char* msg,uint32_t exit_code);

#endif /* HYPERCALLS_H_ */
