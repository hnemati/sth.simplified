#ifndef _HYPER_CONFIG_BASE_H_
#define _HYPER_CONFIG_BASE_H_

#include "hyper.h"
#include "types.h"

/*
 * We need this to know how many modes,
 * memory regions, etc. exist.
 */
#include "hyper_config.h"
#include "hyper_mode_state.h"



/*
 * Guest modes can have different capabilities.
 * These constants enumerate them.
 */

#define HC_CAP_GET_MODE_CONTEXT   0x00000040
#define HC_CAP_SET_MODE_CONTEXT   0x00000080

/*
 * Describes a guest execution mode, including the domain access control
 * bits that are set when entering the mode.
 * Guest modes are "virtual" -- they are logical submodes of the
 * non-privileged CPU mode.
 *
 * There should be 32 or fewer guest modes.
 */
typedef struct hc_guest_mode_ {
	const char* name;
	uint32_t domain_ac;     /* Domain AC bitmap for this mode. */
	uint32_t capabilities; /* Bitmap of what priv operations the mode can do. */
	uint32_t set_mode_contexts; /* Bitmap of guest mode contexts it can set, if capable. */
	uint32_t get_mode_contexts; /* Guest mode contexts it can get, if capable.*/
} hc_guest_mode;


/*
 * Mostly for debugging -- a description of a
 * memory domain.
 */
typedef struct hc_mem_domain_ {
	const char* name;
} hc_mem_domain;


typedef struct hc_interrupt_config_ {
	uint32_t interrupt_mode;
	uint32_t sp;
	interrupt_handler_fn message_handler;
} hc_interrupt_config;


typedef struct hc_errormode_config_ {
	uint32_t error_mode;
	uint32_t sp;
	interrupt_handler_fn error_handler;
} hc_errormode_config;


/*
 * Config structure that the hypervisor will use to
 * set up everything.
 */
typedef struct hc_config_ {
	void (*guest_entry_point)(void);
	uint32_t guest_entry_sp;
	uint32_t guest_entry_mode;
	const hc_mem_domain* mem_domains[16]; /* All 16 ARM MMU domains. */
	const hc_guest_mode* guest_modes[HC_NGUESTMODES];
	hc_interrupt_config interrupt_config;
        hc_errormode_config errormode_config;
} hc_config;

#endif
