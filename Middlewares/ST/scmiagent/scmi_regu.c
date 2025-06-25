/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2015-2023, Arm Limited and Contributors. All rights reserved.
 * Copyright (c) 2023, STMicroelectronics
 *
 * SCMI Voltage Management Protocol Support
 *
 * In the below structure, fields and macro names, _a2p suffix refers to a
 * agent-to-platform message and _p2a suffix refers to a platform-to-agent
 * message. In SMCI specification, the agent is the SCMI client and the
 * platform is the SCMI server.
 */

#include "scmi.h"


enum scmi_voltage_message_id {
	/*
	 * Below message IDs were introduced in SMCI spec v2.0.
	 * Requires SCMI procotol version 0x20000.
	 */
	SCMI_VOLTAGE_PROTO_VERSION = SCMI_PROTOCOL_VERSION,
	SCMI_VOLTAGE_PROTO_ATTRIBS = SCMI_PROTOCOL_ATTRIBUTES,
	SCMI_VOLTAGE_PROTO_MSG_ATTRIBS = SCMI_PROTOCOL_MESSAGE_ATTRIBUTES,
	SCMI_VOLTAGE_DOMAIN_ATTRIBUTES = 0x3,
	SCMI_VOLTAGE_DESCRIBE_LEVELS = 0x4,
	SCMI_VOLTAGE_CONFIG_SET = 0x5,
	SCMI_VOLTAGE_CONFIG_GET = 0x6,
	SCMI_VOLTAGE_LEVEL_SET = 0x7,
	SCMI_VOLTAGE_LEVEL_GET = 0x8,
	SCMI_VOLTAGE_DOMAIN_GET = 0x9,
};

/*
 * ABI for SCMI_PROTOCOL_VERSION message payload
 */

struct scmi_voltage_protocol_version_p2a {
	int32_t status;
	uint32_t version;
};

/*
 * ABI for SCMI_PROTOCOL_ATTRIBUTES message payload
 */
#define SCMI_VOLTAGE_PROTO_ATTRIBS_VOLTAGE_COUNT(_attribs) \
	((_attribs) & 0xffff)

struct scmi_voltage_protocol_attributes_p2a {
	int32_t status;
	uint32_t attributes;
};

/*
 * ABI for SCMI_PROTOCOL_ATTRIBUTES message payload
 */
#define SCMI_VOLTAGE_PROTOCOL_ATTRIBS_VOLTAGE_COUNT_POS			0

#define SCMI_VOLTAGE_PROTOCOL_ATTRIBS_VOLTAGE_COUNT_MASK   \
	(0xffff << SCMI_VOLTAGE_PROTOCOL_ATTRIBS_VOLTAGE_COUNT_POS)

#define SCMI_VOLTAGE_PROTOCOL_ATTRIBS_RESERVED_POS	16

#define SCMI_VOLTAGE_PROTOCOL_ATTRIBS_RESERVED_MASK   \
	(0xffff << SCMI_VOLTAGE_PROTOCOL_ATTRIBS_RESERVED_POS)
/*
 * Mask 0xffff0000 must be reserved and valued to 0
 */
#define SCMI_VOLTAGE_PROTOCOL_ATTRIBUTES(VOLTAGE_COUNT) \
	( \
		((0x0000 <<  SCMI_VOLTAGE_PROTOCOL_ATTRIBS_RESERVED_POS) & \
		  SCMI_VOLTAGE_PROTOCOL_ATTRIBS_RESERVED_MASK) | \
		  (((VOLTAGE_COUNT) << SCMI_VOLTAGE_PROTOCOL_ATTRIBS_VOLTAGE_COUNT_POS) & \
		   SCMI_VOLTAGE_PROTOCOL_ATTRIBS_VOLTAGE_COUNT_MASK) \
	)

struct scmi_protocol_attributes;

/*
 * ABI for PROTOCOL_MESSAGE_ATTRIBUTES message payload
 */

#define SCMI_VOLTAGE_PROTOCOL_MESSAGE_ATTRIBUTES		0

struct scmi_protocol_message_attributes {

};

/*
 * ABI for VOLTAGE_DOMAIN_ATTRIBUTES message payload
 */
#define SCMI_VOLTAGE_DOMAIN_ATTRIBS_EXTENDED_NAME(_enabled)	((_enabled) ? BIT(30) : 0)

#define SCMI_VOLTAGE_DOMAIN_ATTRIBS_ASYNC(_enabled)	((_enabled) ? BIT(31) : 0)

#define SCMI_VOLTAGE_DOMAIN_ATTRIBS_MASK	(0xffff << SCMI_VOLTAGE_DOMAIN_ATTRIBS_POS)

#define SCMI_VOLTAGE_DOMAIN_ATTRIBUTES(EXTENDED_NAME_ENABLE, ASYNC_ENABLE) \
	((SCMI_VOLTAGE_DOMAIN_ATTRIBS_EXTENDED_NAME(EXTENDED_NAME_ENABLE)) | \
	 (SCMI_VOLTAGE_DOMAIN_ATTRIBS_ASYNC(ASYNC_ENABLE)) \
	)

#define SCMI_VOLTAGE_NAME_LENGTH_MAX		16

struct scmi_voltage_attributes_a2p {
    uint32_t domain_id;
};

struct scmi_voltage_attributes_p2a {
    int32_t status;
    uint32_t attributes;
    char name[SCMI_VOLTAGE_NAME_LENGTH_MAX];
};

/*
 * Voltage Describe Levels
 */

struct scmi_voltage {

};

struct scmi_voltage_describe_levels_a2p {
	uint32_t domain_id;
	uint32_t level_index;
};

struct scmi_voltage_describe_levels_p2a {
	int32_t status;
	uint32_t flags;
	struct scmi_voltage voltages[];
};

/*
 * ABI for VOLTAGE_CONFIG_SET message payload
 */

enum scmi_voltd_mode_type {
    SCMI_VOLTD_MODE_TYPE_ARCH = 0x0U,
    SCMI_VOLTD_MODE_TYPE_IMPL = 0x08U,
};

enum scmi_voltd_mode_id {
    SCMI_VOLTD_MODE_ID_OFF = 0x0U,
    SCMI_VOLTD_MODE_ID_ON = 0x07U,
};

#define SCMI_VOLTAGE_CONFIG_MODE(_arch)	((_arch) ? \
                                         SCMI_VOLTD_MODE_TYPE_IMPL : \
					 SCMI_VOLTD_MODE_TYPE_ARCH)

#define SCMI_VOLTAGE_CONFIG_MODE_ID_MASK	GENMASK(2, 0)

#define SCMI_VOLTAGE_CONFIG_MODE_ID_ENABLE(_enable) ((_enable) ? \
                                                     SCMI_VOLTD_MODE_ID_ON : \
						     SCMI_VOLTD_MODE_ID_OFF)
#define TEST(_enable) \
	((_enable) ? TES_TEST : 0)

#define SCMI_VOLTAGE_CONFIG_SET_CONFIG(_arch, _enable) \
	(SCMI_VOLTAGE_CONFIG_MODE_ID_ENABLE(_enable) | \
	 SCMI_VOLTAGE_CONFIG_MODE(_arch))

struct scmi_voltage_config_set_a2p {
    uint32_t domain_id;
    uint32_t config;
};

struct scmi_voltage_config_set_p2a {
    int32_t status;
};

/*
 * ABI for VOLTAGE_CONFIG_GET message payload
 */

struct scmi_voltage_config_get_a2p {
    uint32_t domain_id;
};

struct scmi_voltage_config_get_p2a {
    int32_t status;
    uint32_t config;
};

/*
 * ABI for VOLTAGE_LEVEL_SET message payload
 */

#define SCMI_VOLTAGE_LEVEL_SET_FLAGS_ASYNC(_enabled)	((_enabled) ? BIT(0) : 0)

struct scmi_voltage_level_set_a2p {
    uint32_t domain_id;
    uint32_t flags;
    int32_t voltage_level;
};

struct scmi_voltage_level_set_p2a {
    int32_t status;
};

/*
 * ABI for VOLTAGE_LEVEL_GET message payload
 */

struct scmi_voltage_level_get_a2p {
    uint32_t domain_id;
};

struct scmi_voltage_level_get_p2a {
    int32_t status;
    int32_t voltage_level;
};

/*
 * ABI for VOLTAGE_DOMAIN_NAME_GET message payload
 */

struct scmi_voltage_domain_name_get_a2p {
	uint32_t domain_id;
};

#define SCMI_VOLTAGE_EXTENDED_NAME_LENGTH_MAX 64

struct scmi_clock_name_get_p2a {
	int32_t status;
	uint32_t flags;
	char clock_name[SCMI_VOLTAGE_EXTENDED_NAME_LENGTH_MAX];
};

struct scmi_channel scmi_channel =
{
  .tx_buffer     = (uint32_t *)(SCMI_SHM_START_ADDRESS),
  .rx_buffer     = (uint32_t *)(SCMI_SHM_START_ADDRESS + SMT_SHARED_MEMORY_SIZE),
};

/*
 * Enabling and disabling a voltage domain
 */
static int scmi_pm_gate(struct scmi_channel *channel,
			unsigned int domain_id,
			int enable)
{
	struct scmi_voltage_config_set_p2a response = { };
	struct scmi_voltage_config_set_a2p message = {
		.domain_id = domain_id,
		.config = SCMI_VOLTAGE_CONFIG_SET_CONFIG(0, enable),
	};
	struct scmi_message_data a2p = {
		.channel = channel,
		.protocol_id = SCMI_PROTOCOL_ID_VOLTAGE_DOMAIN,
		.message_id = SCMI_VOLTAGE_CONFIG_SET,
		.message = &message,
		.message_size = sizeof(message),
		.response = &response,
		.response_size = sizeof(response),
	};
	int ret = 0;


	ret = scmi_process_message(&a2p);
	if (ret)
		return ret;

	return scmi_status_to_ret(response.status);
}

/* Function to register to regulator framework */
int scmi_voltage_domain_enable(struct scmi_channel *channel, unsigned int domain_id)
{
	return scmi_pm_gate(channel, domain_id, 1);
}

/* Function to register to clock framework */
int scmi_voltage_domain_disable(struct scmi_channel *channel, unsigned int domain_id)
{
	return scmi_pm_gate(channel, domain_id, 0);
}
