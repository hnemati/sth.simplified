#include "hw.h"
#include "hyper_mode_state.h"

#include "lib.h"
#include "hypercalls.h"

extern virtual_machine *curr_vm;

// Change hypervisor variables and MMU domain AC.
void change_guest_mode (uint32_t mode)
{
	uint32_t domac;
	curr_vm->current_mode_state = &curr_vm->mode_states[mode];
	cpu_context_current_set(&(curr_vm->current_mode_state->ctx));
	curr_vm->current_guest_mode = mode;
	domac = curr_vm->current_mode_state->mode_config->domain_ac;
	mem_mmu_set_domain(domac);
}

void hypercall_message(uint32_t param)
{
  curr_vm->next->b_new_message = 1;
  curr_vm->next->message = param;
  printf("msg is %x\n", param);
}

/*
 * Setup Guest VM with hypervisor
 */
void hypercall_end_interrupt () {
  if (curr_vm->current_guest_mode == HC_GM_TASK)
    change_guest_mode(HC_GM_INTERRUPT);
  else
    change_guest_mode(HC_GM_TASK);
}

/*
 * Set guest mode execution context.
 */
/*
void hypercall_set_mode_context (set_mode_context_args* args)
{

	if (args->mode >= HC_NGUESTMODES)
	{
		hyper_panic("Guest tried to set mode context for an invalid mode.", 1);
	}
	if (!(curr_vm->current_mode_state->mode_config->set_mode_contexts & (1 << args->mode)))
	{
		printf("mode:%d\n",args->mode);
		printf("setModeContexts:%d", curr_vm->current_mode_state->mode_config->set_mode_contexts);
		hyper_panic("Guest tried to set mode context without proper permissions.", 2);
	}
	// TODO: verify origin and buffer address
	curr_vm->mode_states[args->mode].ctx = *(args->ctx);
}


/*
 * Get execution context for a particular guest mode.
 */
/*
void hypercall_get_mode_context (get_mode_context_args* args)
{
	if (args->mode >= HC_NGUESTMODES)

	{
		hyper_panic("Guest tried to get mode context for an invalid mode.", 1);
	}
	if (!(curr_vm->current_mode_state->mode_config->get_mode_contexts & (1 << args->mode)))
	{
		hyper_panic("Guest tried to get mode context without proper permissions.", 2);
	}
	// TODO: verify origin and buffer address
	*(args->ctx) = curr_vm->mode_states[args->mode].ctx;
}
*/
//	ERROR HANDLING CODE -----------------------------------

void hypercall_num_error (uint32_t hypercall_num)
{

	uint32_t addr = (curr_vm->current_mode_state->ctx.pc -4);
	//printf ("Unknown hypercall %d originated at 0x%x, aborting", hypercall_num, addr);
	
}

void hyper_panic(char* msg, uint32_t exit_code) {
  //printf("\n\n***************************************\n"
  //		       "HYPERVISOR PANIC: \n"
  //			   "%s\n"
  //			   "***************************************\n", msg);
	//	terminate(exit_code);
}
