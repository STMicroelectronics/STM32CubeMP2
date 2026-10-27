/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2025, STMicroelectronics
 *
 * SCMI System Power Management Protocol Support (Protocol ID 0x12)
 *
 * In the below structure, fields and macro names, _a2p suffix refers to an
 * agent-to-platform message and _p2a suffix refers to a platform-to-agent
 * message. In SCMI specification, the agent is the SCMI client and the
 * platform is the SCMI server.
 *
 * Reference: SCMI Specification (DEN0056), System Power Management Protocol.
 */

#include "scmi.h"

/* -------------------------------------------------------------------------
 * SCMI System Power Protocol Message IDs (Protocol 0x12)
 * ------------------------------------------------------------------------- */
enum scmi_sys_power_message_id {
	SCMI_SYS_POWER_PROTO_VERSION        = SCMI_PROTOCOL_VERSION,  /* 0x0 */
	SCMI_SYS_POWER_PROTO_ATTRIBS        = SCMI_PROTOCOL_ATTRIBUTES, /* 0x1 */
	SCMI_SYS_POWER_PROTO_MSG_ATTRIBS    = SCMI_PROTOCOL_MESSAGE_ATTRIBUTES, /* 0x2 */
	SCMI_SYS_POWER_STATE_SET            = 0x3,
	SCMI_SYS_POWER_STATE_NOTIFY         = 0x5,
};

/* -------------------------------------------------------------------------
 * System power state values (matches enum scmi_sys_power in tfm_scmi_api.h)
 * ------------------------------------------------------------------------- */
#define SCMI_SYS_STATE_SHUTDOWN             0U
#define SCMI_SYS_STATE_COLD_RESET           1U
#define SCMI_SYS_STATE_WARM_RESET           2U
#define SCMI_SYS_STATE_SUSPEND              4U

/* -------------------------------------------------------------------------
 * SYSTEM_POWER_STATE_SET (message_id = 0x3)
 *
 * flags[0] = 0 → forceful transition
 * flags[0] = 1 → graceful transition (server notifies agents before acting)
 * ------------------------------------------------------------------------- */
#define SCMI_SYS_POWER_STATE_SET_FLAG_GRACEFUL  BIT(0)

struct scmi_sys_power_state_set_a2p {
	uint32_t flags;         /* bit[0]: graceful */
	uint32_t system_state; /* SCMI_SYS_STATE_* */
};

struct scmi_sys_power_state_set_p2a {
	int32_t status;
};

/* =========================================================================
 * Public API
 * ========================================================================= */

/**
 * scmi_sys_power_state_set() - Request the SCMI server to perform a system
 *                              power state transition.
 *
 * @channel:      SCMI channel to use (must be already initialised).
 * @system_state: Requested state — use SCMI_SYS_STATE_* defines above
 *                (COLD_RESET = 1, WARM_RESET = 2, SHUTDOWN = 0, SUSPEND = 4).
 * @graceful:     Non-zero → ask the server to notify agents before acting.
 *                Zero    → immediate / forceful transition.
 *
 * Returns 0 on success, a negative scmiagent_error otherwise.
 *
 * NOTE: On success the server will start the requested power transition.
 *       For SYS_POWER_COLD_RESET the CA35 core will be reset — the calling
 *       code should not expect further execution beyond this call.
 */
int scmi_sys_power_state_set(struct scmi_channel *channel,
                             uint32_t system_state,
                             int graceful)
{
	struct scmi_sys_power_state_set_p2a response = { 0 };
	struct scmi_sys_power_state_set_a2p message = {
		.flags        = graceful ? SCMI_SYS_POWER_STATE_SET_FLAG_GRACEFUL : 0U,
		.system_state = system_state,
	};
	struct scmi_message_data a2p = {
		.channel       = channel,
		.protocol_id   = SCMI_PROTOCOL_ID_SYSTEM,
		.message_id    = SCMI_SYS_POWER_STATE_SET,
		.message       = &message,
		.message_size  = sizeof(message),
		.response      = &response,
		.response_size = sizeof(response),
	};
	int ret;

	ret = scmi_process_message(&a2p);
	if (ret)
		return ret;

	return scmi_status_to_ret(response.status);
}
