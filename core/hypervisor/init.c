#include <hw.h>
#include <lib.h>

#include "hyper_mode_state.h"
#include "hyper_config_base.h"
/*
 * Function prototypes
 */
void change_guest_mode();

//Handlers
void prog_abort_handler();
void data_abort_handler();
void swi_handler();
void irq_handler();
void undef_handler();



/****************************/

/*
 * Globals
 */

static uint32_t flpt[4096] __attribute__ ((aligned (16 * 1024)));

//Static VM - May change to dynamic in future
virtual_machine vm_0;
virtual_machine vm_1;
virtual_machine *curr_vm;

extern void start_();
extern hc_config minimal_config[];
/*****************************/

void memory_init()
{
    
	uint32_t i, domain = 0, ap = 0, pt = 0;

    cpu_type type;
    cpu_model model;

    cpu_get_type(&type, &model);

    /* Start with simple access control
     * Only seperation between hypervisor and user address space
     */
    for(i = 0; i < 4096; i++) {
        /* FIXME: 1 1MB section can contain multiple entries, here we will just see the first one */
        memory_layout_entry * mle = (memory_layout_entry*)mem_padr_lookup( i << 20);
        uint32_t mem_type = mle ? mle->type : MLT_NONE;

        switch(mem_type) {
        case MLT_HYPER_RAM:
            domain = 0;
            ap = 1; //Only privileged RW access
            break;
        case MLT_IO_REG:
            domain = 0;
            ap = 3; // Give guest permission to all IO registers for now
            break;
        case MLT_USER1_RAM:
            domain = 1;
            ap = 3; //User access RW
            break;
        case MLT_USER2_RAM:
            domain = 2;
            ap = 3; //User access RW
            break;
        default:
            // unknown stuff, no access
            domain = 2;
            ap = 0;
        }

        if(type == CT_ARM && model == CM_ARMv7a) {

            // ARM Cortex-An
            pt = (i << 20) | (domain << 5) | (ap << 10) | 0x012;
            // printf("pt: %x\n", pt);
            // if RAM, turn of XN and enable cache and buffer
            if(mem_type == MLT_HYPER_RAM || mem_type == MLT_USER1_RAM || mem_type == MLT_USER2_RAM)
                pt = (pt & (~0x10)) | 0xC;
        }

        flpt[i] = pt;
    }
    mem_mmu_set_domain(0x55555555); // Start with access to all domains
    mem_mmu_set_translation_table(flpt);
    mem_mmu_tlb_invalidate_all(TRUE, TRUE);
    printf("test 5\n");
    CP15MMUEnable();
    printf("test6\n");
}

void setup_handlers()
{
	//Direct the exception to the hypervisor handlers
	cpu_set_abort_handler((cpu_callback)prog_abort_handler, (cpu_callback)data_abort_handler);
	cpu_set_swi_handler((cpu_callback)swi_handler);
	cpu_set_undef_handler((cpu_callback)undef_handler);

	// Start the timer and direct interrupts to hypervisor irq handler
	timer_tick_start((cpu_callback)irq_handler);
}

void guests_init()
{
	uint32_t i;
	virtual_machine *tmp;

	vm_0.id = 0;
	vm_0.next = &vm_1;
	vm_0.config = &(minimal_config[0]);

	vm_1.id = 1;
	vm_1.next = &vm_0;
	vm_1.config = &(minimal_config[1]);

	//Start with VM_0 as the current VM
	curr_vm = &vm_0;

	tmp = &vm_0;
	//Iterate through all VMs
	do{
		for(i = 0; i < HC_NGUESTMODES;i++){
			// Default values for guest mode setup
			curr_vm->mode_states[i].mode_config = 0;
			// Setup guest modes
			curr_vm->mode_states[i].mode_config = (curr_vm->config->guest_modes[i]);
			curr_vm->mode_states[i].ctx.reg[0] = 0;
			curr_vm->mode_states[i].ctx.reg[1] = 0;
			curr_vm->mode_states[i].ctx.reg[2] = 0;
			curr_vm->mode_states[i].ctx.reg[3] = 0;
			curr_vm->mode_states[i].ctx.reg[4] = 0;
			curr_vm->mode_states[i].ctx.reg[5] = 0;
			curr_vm->mode_states[i].ctx.reg[6] = 0;
			curr_vm->mode_states[i].ctx.reg[7] = 0;
			curr_vm->mode_states[i].ctx.reg[8] = 0;
			curr_vm->mode_states[i].ctx.reg[9] = 0;
			curr_vm->mode_states[i].ctx.reg[10] = 0;
			curr_vm->mode_states[i].ctx.reg[11] = 0;
			curr_vm->mode_states[i].ctx.reg[12] = 0;
			curr_vm->mode_states[i].ctx.lr = 0;
		}
		//Init default values
		curr_vm->current_guest_mode = HC_GM_TASK;
		curr_vm->interrupted_mode = HC_GM_INTERRUPT;
		curr_vm->current_mode_state = &(curr_vm->mode_states[HC_GM_TASK]);

		//Setup VM task mode context (entry registers)
		curr_vm->mode_states[HC_GM_TASK].ctx.pc = (uint32_t)curr_vm->config->guest_entry_point;
		curr_vm->mode_states[HC_GM_TASK].ctx.sp = curr_vm->config->guest_entry_sp;
		curr_vm->mode_states[HC_GM_TASK].ctx.psr= 0x10; //interrupts off

		curr_vm->mode_states[HC_GM_INTERRUPT].ctx.pc = (uint32_t)curr_vm->config->interrupt_config.message_handler;
		curr_vm->mode_states[HC_GM_INTERRUPT].ctx.sp = (uint32_t)curr_vm->config->interrupt_config.sp;
		curr_vm->mode_states[HC_GM_INTERRUPT].ctx.psr = 0x10; //interrupts off
		//Set guests tick handler
		//curr_vm->guest_tick_handler = curr_vm->config->interrupt_config.tick_handler;

		curr_vm->b_new_message = 0;
		curr_vm->message = 0;

		curr_vm = curr_vm->next;
	}while(curr_vm != &vm_0);
 end_guests_init_loop:
	cpu_context_initial_set(&curr_vm->mode_states[HC_GM_TASK].ctx);

}

void start_guest()
{
    printf("start_guest\n");
	//Change guest mode to TASK before going into guest
	change_guest_mode(HC_GM_TASK);
	SET_USER_SP(curr_vm->config->guest_entry_sp);
	asm ("mov LR, %0      \n\t" :: "r"(curr_vm->config->guest_entry_point));
	printf("guest entry point %x\n",curr_vm->config->guest_entry_point);
	asm (
	    // set the SPSR to user mode, interrupts disabled (for startup)
		"MSR SPSR_fsxc, #0x10 \n\t" 	 //USR MODE INTERRUPTS ENABLED
		//"MSR SPSR, #0xD0 \n\t" //USR MODE INTERRUPTS DISABLED
		"MOV  R0, #0  \n\t"
		"MOV  R1, #0  \n\t"
		"MOV  R2, #0  \n\t"
		"MOV  R3, #0  \n\t"
		"MOV  R4, #0  \n\t"
		"MOV  R5, #0  \n\t"
		"MOV  R6, #0  \n\t"
		"MOV  R7, #0  \n\t"
		"MOV  R8, #0  \n\t"
		"MOV  R9, #0  \n\t"
		"MOV  R10, #0  \n\t"
		"MOV  R11, #0  \n\t"
		"MOV  R12, #0  \n\t"
		"MOV  R12, #0  \n\t"
		"MOVS  PC, LR  \n\t" //MOVS restores the CPSR as well as changing the PC
	);
    printf("end guest\n");
}

void init_()
{
	/* CPU specific initialization	 */
	cpu_init();
	/* Initialize minimum essential SOC peripherals */
	printf("test1\n");
    soc_init();

	/*Setting up pagetable rules and enabling MMU*/
	//printf("test2\n");
    memory_init();
	//printf("test3\n");
    /* Setting up exception handlers and starting timer*/
	setup_handlers();
    //printf("test4\n");
	/* Initialize hypervisor guest modes and data structures
	 * according to config file in guest*/
	guests_init();

	//printf("Hypervisor initialized\n Entering Guest\n");
	start_guest();
}
