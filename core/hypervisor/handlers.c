#include "hw.h"
#include "lib.h"

#include "hypercalls.h"
#include "hyper_mode_state.h"
/*
 * Exception Handlers
 */

void change_guest_mode(uint32_t);


extern virtual_machine *curr_vm;

/*
 * Hypercall handler
 * @param0 			 - No use at the moment
 * @param  			 - void pointer to anything, usually a struct
 * @hypercall_number - Hypercall to be executed
 */



void reset_guest()
{
  int i;
		for(i = 0; i < HC_NGUESTMODES;i++){
			// Default values for guest mode setup
			curr_vm->mode_states[i].mode_config = 0;
			// Setup guest modes
			curr_vm->mode_states[i].mode_config = (curr_vm->config->guest_modes[i]);
		}
		//Init default values
		curr_vm->current_guest_mode = MODE_NONE;
		curr_vm->interrupted_mode = MODE_NONE;
		curr_vm->current_mode_state = 0;

		//Setup VM task mode context (entry registers)
		curr_vm->mode_states[HC_GM_TASK].ctx.pc = (uint32_t)curr_vm->config->guest_entry_point;
		curr_vm->mode_states[HC_GM_TASK].ctx.sp = curr_vm->config->guest_entry_sp;
		curr_vm->mode_states[HC_GM_TASK].ctx.psr= 0xD0; //interrupts off

		//Set guests tick handler
		//curr_vm->guest_tick_handler = curr_vm->config->interrupt_config.tick_handler;

		// TODO for all guest modes
		// clear memory
		// clear registers
	change_guest_mode(HC_GM_TASK);
}

void swi_handler(uint32_t param0, void *param, uint32_t hypercall_number)
{
        printf("Hypercall number:%d called\n", hypercall_number);
	switch(hypercall_number){
	case HYPERCALL_MESSAGE:
	  hypercall_message(param);
	  return;
	case HYPERCALL_RELEASE:
	  hypercall_end_interrupt();
	//hypercall_release();
          return;
	default:
	  return;
	}
}
 
return_value prog_abort_handler(uint32_t addr, uint32_t status, uint32_t unused)
{
  //printf("Prog abort\n Address:%x Status:%x \n", addr, status);
  return RV_OK;
}

return_value data_abort_handler(uint32_t addr, uint32_t status, uint32_t unused)
{
  //printf("Data abort\n Address:%x Status:%x \n", addr, status);
    return RV_OK;
}


void irq_handler()
{
	uint32_t psr;
	//printf("IRQ handler called\n");

	virtual_machine * other = curr_vm->next;
	if (other->b_new_message && other->current_guest_mode==HC_GM_TASK)
	  {
	    other->b_new_message = 0;
	    curr_vm->interrupted_mode = curr_vm->current_guest_mode;
	    // old error
	    //psr = cpu_interrupt_user_set(INTERRUPT_MASK); //returns the modified psr
	    curr_vm= curr_vm->next;
	
	    //Give execution to guest tick handler (interrupt mode)
	    change_guest_mode(curr_vm->config->interrupt_config.interrupt_mode);
	    curr_vm->current_mode_state->ctx.pc = (uint32_t)curr_vm->config->interrupt_config.message_handler;
	    //curr_vm->current_mode_state->ctx.psr = psr;
	    curr_vm->current_mode_state->ctx.reg[0] = curr_vm->message;
	    //curr_vm->current_mode_state->ctx.sp = curr_vm->config->interrupt_config.sp;
	    // whitout changing the mode of the guest and changing its pc

	    return;
	  }
	curr_vm= curr_vm->next;

	uint32_t domac = domac = curr_vm->current_mode_state->mode_config->domain_ac;
	cpu_context_current_set(&(curr_vm->current_mode_state->ctx));

	mem_mmu_set_domain(domac);
}


return_value undef_handler(uint32_t instr, uint32_t unused, uint32_t addr)
{
  //printf("Undefined abort\n Address:%x Instruction:%x \n", addr, instr);
    // if was running a guest

    return RV_OK;
}
