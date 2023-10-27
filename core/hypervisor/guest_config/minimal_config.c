#include "hyper_config_base.h"

#define HC_DOM_HYPER 0
#define HC_DOM_GUEST_1 1
#define HC_DOM_GUEST_2 2

/*
 * Bitmask constants for specifying guest mode
 * contexts that can be get/set.
 */
#define HC_GM_GUEST_1_MASK   (1 << HC_GM_GUEST_1)
#define HC_GM_GUEST_2_MASK    (1 << HC_GM_GUEST_2)

/*
 * Guest mode access to certain domains
 * ********************************************************/

#define HC_DOMAC_ALL \
	((1 << (2 * HC_DOM_GUEST_1)) | \
	(1 << (2 * HC_DOM_GUEST_2))  | \
	(1 << (2 * HC_DOM_HYPER)))

#define HC_DOMAC_GUEST_1 \
	(1 << (2 * HC_DOM_GUEST_1)  | \
	(1 << (2 * HC_DOM_HYPER)))

#define HC_DOMAC_GUEST_2 \
	(1 << (2 * HC_DOM_GUEST_2)  | \
	(1 << (2 * HC_DOM_HYPER)))

/*************************************************************/

/*
 * Guest mode capabilities
 */
#define HC_CAP_TASK 0	// No capabilities

/*************************************************************/

/*
 * Configuration for guest modes
 */

static const hc_guest_mode
	gm_interrupt_1 = {
			.name = "interrupt domain1",
			.domain_ac = HC_DOMAC_GUEST_1,
			.capabilities = HC_CAP_TASK,
			.set_mode_contexts = 0,
			.get_mode_contexts = 0
	},
	gm_task_1 = {
			.name = "application domain1",
			.domain_ac = HC_DOMAC_GUEST_1,
			.capabilities = HC_CAP_TASK,
			.set_mode_contexts = 0,
			.get_mode_contexts = 0
	},
	gm_interrupt_2 = {
			.name = "interrupt domain2",
			.domain_ac = HC_DOMAC_GUEST_2,
			.capabilities = HC_CAP_TASK,
			.set_mode_contexts = 0,
			.get_mode_contexts = 0
	},
	gm_task_2 = {
			.name = "application domain2",
			.domain_ac = HC_DOMAC_GUEST_2,
			.capabilities = HC_CAP_TASK,
			.set_mode_contexts = 0,
			.get_mode_contexts = 0
	};

/*
 * Guest configuration structure
 */

hc_config minimal_config[2] = {
  {
		.guest_entry_point = (void*)0x100000,
		.guest_entry_sp	 = 0x300000 - 4,
		.guest_entry_mode  = HC_GM_TASK,
		.guest_modes = {&gm_interrupt_1, &gm_task_1},
		.interrupt_config = {
                                .message_handler = (void*)0x200000,
				.interrupt_mode = HC_GM_INTERRUPT,
				.sp = 0x400000-4,//LINKER_SYM_UINT(__interrupt_sp__),
    }},
  {
		.guest_entry_point = (void*)0x400000,
		.guest_entry_sp	 = 0x600000 - 4,
		.guest_entry_mode  = HC_GM_TASK,
		.guest_modes = {&gm_interrupt_2, &gm_task_2},
		.interrupt_config = {
                                .message_handler = (void*)0x500000,
				.interrupt_mode = HC_GM_INTERRUPT,
				.sp = 0x700000-4,//LINKER_SYM_UINT(__interrupt_sp__),
    }},
};
