#ifndef _HYPER_MODE_STATE_H_
#define _HYPER_MODE_STATE_H_

#include "hyper_config_base.h"
#include "arm_common.h"

typedef struct hyper_mode_state_ {
	context ctx;
	const struct hc_guest_mode_ *mode_config;
} hyper_mode_state;

typedef struct virtual_machine_{
	uint32_t id;
	uint32_t current_guest_mode;
	uint32_t interrupted_mode;
	hyper_mode_state mode_states[HC_NGUESTMODES];
	hyper_mode_state *current_mode_state;
	struct hc_config_ *config;
	struct virtual_machine_ *next;
  uint32_t message;
  uint32_t b_new_message;
} virtual_machine;

#endif
