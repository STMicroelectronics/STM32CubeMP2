/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2015-2023, Arm Limited and Contributors. All rights reserved.
 * Copyright (c) 2023, STMicroelectronics
 *
 * SCMI Clock Management Protocol Support
 *
 * In the below structure, fields and macro names, _a2p suffix refers to a
 * agent-to-platform message and _p2a suffix refers to a platform-to-agent
 * message. In SMCI specification, the agent is the SCMI client and the
 * platform is the SCMI server.
 */

#include "scmi.h"

#define SCMI_CLOCK_NAME_LENGTH_MAX		16

/*
 * struct scmi_clock - Private data of a clock driven through SCMI
 * @channel: Reference to the SCMI channel used to access the SCMI clock
 * @scmi_id: ID of the SCMI clock ID exposed on the SCMI channel
 * @name: Name of the clock as exposed by the SCMI server
 */
struct scmi_clock {
	struct scmi_channel *channel;
	unsigned int clock_id;
	char name[SCMI_CLOCK_NAME_LENGTH_MAX];
};

enum scmi_clock_message_id {
	/*
	 * Below message IDs were introduced in SMCI spec v1.0.
	 * Requires SCMI procotol version 0x10000.
	 */
	SCMI_CLOCK_PROTO_VERSION = SCMI_PROTOCOL_VERSION,
	SCMI_CLOCK_PROTO_ATTRIBS = SCMI_PROTOCOL_ATTRIBUTES,
	SCMI_CLOCK_PROTO_MSG_ATTRIBS = SCMI_PROTOCOL_MESSAGE_ATTRIBUTES,
	SCMI_CLOCK_ATTRIBUTES = 0x3,
	SCMI_CLOCK_DESCRIBE_RATES = 0x4,
	SCMI_CLOCK_RATE_SET = 0x5,
	SCMI_CLOCK_RATE_GET = 0x6,
	SCMI_CLOCK_CONFIG_SET = 0x7,
	/*
	 * Below message IDs were introduced in SMCI spec v3.1.
	 * Requires SCMI procotol version 0x20000.
	 */
	SCMI_CLOCK_CONFIG_GET = 0x8,
	SCMI_CLOCK_NAME_GET = 0x9,
	SCMI_CLOCK_RATE_NOTIFY = 0xa,
	SCMI_CLOCK_RATE_CHANGE_NOTIFY = 0xb,
	/*
	 * Below message IDs were introduced in SMCI spec v3.2.
	 * Requires SCMI procotol version 0x20001.
	 */
	SCMI_CLOCK_POSSIBLE_PARENTS_GET = 0xc,
	SCMI_CLOCK_PARENT_SET = 0xd,
	SCMI_CLOCK_PARENT_GET = 0xe,
};

/*
 * ABI for SCMI_PROTOCOL_VERSION message payload
 */

struct scmi_clock_protocol_version_p2a {
	int32_t status;
	uint32_t version;
};

/*
 * ABI for SCMI_PROTOCOL_ATTRIBUTES message payload
 */
#define SCMI_CLOCK_PROTO_ATTRIBS_CLOCK_COUNT(_attribs) \
	((_attribs) & 0xffff)

#define SCMI_CLOCK_PROTO_ATTRIBS_MAX_PENDING_RATE_CHANGE(_attribs) \
	(((_attribs) >> 16) & 0xff)

struct scmi_clock_protocol_attributes_p2a {
	int32_t status;
	uint32_t attributes;
};

/*
 * ABI for PROTOCOL_ATTRIBUTES message payload
 */
#define SCMI_CLOCK_PROTOCOL_ATTRIBUTES_MAX_PENDING_TRANSITIONS_POS	16

#define SCMI_CLOCK_PROTOCOL_ATTRIBUTES_CLOCK_COUNT_POS			0

#define SCMI_CLOCK_PROTOCOL_ATTRIBUTES_MAX_PENDING_TRANSITIONS_MASK \
	(0xff << SCMI_CLOCK_PROTOCOL_ATTRIBUTES_MAX_PENDING_TRANSITIONS_POS)

#define SCMI_CLOCK_PROTOCOL_ATTRIBUTES_CLOCK_COUNT_MASK   \
	(0xffff << SCMI_CLOCK_PROTOCOL_ATTRIBUTES_CLOCK_COUNT_POS)

#define SCMI_CLOCK_PROTOCOL_ATTRIBUTES(MAX_PENDING_TRANSACTIONS, CLOCK_COUNT) \
	( \
		((MAX_PENDING_TRANSACTIONS << \
		  SCMI_CLOCK_PROTOCOL_ATTRIBUTES_MAX_PENDING_TRANSITIONS_POS) & \
		  SCMI_CLOCK_PROTOCOL_ATTRIBUTES_MAX_PENDING_TRANSITIONS_MASK) | \
		  (((CLOCK_COUNT) << SCMI_CLOCK_PROTOCOL_ATTRIBUTES_CLOCK_COUNT_POS) & \
		   SCMI_CLOCK_PROTOCOL_ATTRIBUTES_CLOCK_COUNT_MASK) \
	)

struct scmi_protocol_attributes;

/*
 * ABI for PROTOCOL_MESSAGE_ATTRIBUTES message payload
 */
#define SCMI_CLOCK_PROTOCOL_MESSAGE_ATTRIBUTES		0

struct scmi_protocol_message_attributes {

};

/*
 * ABI for CLOCK_ATTRIBUTES message payload
 */
#define SCMI_CLOCK_ATTRIBUTES(_enabled)		((_enabled) ? BIT(0) : 0)

struct scmi_clock_attributes_a2p {
    uint32_t clock_id;
};

struct scmi_clock_attributes_p2a {
    int32_t status;
    uint32_t attributes;
    char clock_name[SCMI_CLOCK_NAME_LENGTH_MAX];
};

/*
 * ABI for CLOCK_RATE_SET message payload
 */
#define SCMI_CLOCK_RATE_SET_ROUND_MASK		(3 << 2)
#define SCMI_CLOCK_RATE_SET_ROUND_UP		(1 << 2)
#define SCMI_CLOCK_RATE_SET_ROUND_DOWN		(0 << 2)
#define SCMI_CLOCK_RATE_SET_ROUND_CLOSEST	(2 << 2)

#define SCMI_CLOCK_RATE_SET_ASYNC(_enable)	((_enable) ? BIT(0) : 0)

#define SCMI_CLOCK_RATE_SET_NO_DELAYED_RESPONSE(_enable)	((_enable) ? BIT(1) : 0)

struct scmi_clock_rate {
	uint32_t lsb;
	uint32_t msb;
};

struct scmi_clock_rate_set_a2p {
	uint32_t flags;
	uint32_t clock_id;
	struct scmi_clock_rate rate;
};

struct scmi_clock_rate_set_p2a {
	int32_t status;
};

/*
 * ABI for CLOCK_RATE_GET message payload
 */

struct scmi_clock_rate_get_a2p {
	uint32_t clock_id;
};

struct scmi_clock_rate_get_p2a {
	int32_t status;
	struct scmi_clock_rate rate;
};

/*
 * ABI for CLOCK_CONIG_SET message payload
 */

#define SCMI_CLOCK_CONFIG_SET_ENABLE_POS	0

#define SCMI_CLOCK_CONFIG_SET_ENABLE(_enable) \
	((_enable) << SCMI_CLOCK_CONFIG_SET_ENABLE_POS)

struct scmi_clock_config_set_a2p {
    uint32_t clock_id;
    uint32_t attributes;
};

struct scmi_clock_config_set_p2a {
    int32_t status;
};

/*
 * Clock Describe Rates
 */

#define SCMI_CLOCK_RATE_FORMAT_RANGE 1U
#define SCMI_CLOCK_RATE_FORMAT_LIST  0U

#define SCMI_CLOCK_NUM_OF_RATES_RANGE 3U

#define SCMI_CLOCK_DESCRIBE_RATES_REMAINING_POS 16U
#define SCMI_CLOCK_DESCRIBE_RATES_FORMAT_POS    12U
#define SCMI_CLOCK_DESCRIBE_RATES_COUNT_POS     0U

#define SCMI_CLOCK_DESCRIBE_RATES_NUM_RATES_FLAGS( \
	RATE_COUNT, RETURN_FORMAT, REMAINING_RATES) \
	( \
	 ((RATE_COUNT << \
	   SCMI_CLOCK_DESCRIBE_RATES_COUNT_POS) & \
	  SCMI_CLOCK_DESCRIBE_RATES_COUNT_MASK) | \
	 ((REMAINING_RATES << SCMI_CLOCK_DESCRIBE_RATES_REMAINING_POS) & \
	  SCMI_CLOCK_DESCRIBE_RATES_REMAINING_MASK) | \
	 ((RETURN_FORMAT << SCMI_CLOCK_DESCRIBE_RATES_FORMAT_POS) & \
	  SCMI_CLOCK_DESCRIBE_RATES_FORMAT_MASK) \
	)

#define SCMI_CLOCK_DESCRIBE_RATES_REMAINING_MASK \
	(0xFFFF << SCMI_CLOCK_DESCRIBE_RATES_REMAINING_POS)

#define SCMI_CLOCK_DESCRIBE_RATES_FORMAT_MASK \
	(0x1 << SCMI_CLOCK_DESCRIBE_RATES_FORMAT_POS)

#define SCMI_CLOCK_DESCRIBE_RATES_COUNT_MASK \
	(0xFFF << SCMI_CLOCK_DESCRIBE_RATES_COUNT_POS)

#define SCMI_CLOCK_RATES_MAX(MAILBOX_SIZE) \
	((sizeof(struct scmi_clock_describe_rates_p2a) < (MAILBOX_SIZE))  ? \
	 (((MAILBOX_SIZE) - sizeof(struct scmi_clock_describe_rates_p2a)) / \
	  sizeof(struct scmi_clock_rate)) : \
	 0)

struct scmi_clock_describe_rates_a2p {
	uint32_t clock_id;
	uint32_t rate_index;
};

struct scmi_clock_describe_rates_p2a {
	int32_t status;
	uint32_t num_rates_flags;
	struct scmi_clock_rate rates[];
};

/*
 * Clock get name (extended 64byte name)
 */

struct scmi_clock_name_get_a2p {
	uint32_t clock_id;
};

#define SCMI_CLOCK_EXTENDED_NAME_LENGTH_MAX 64

struct scmi_clock_name_get_p2a {
	int32_t status;
	uint32_t flags;
	char clock_name[SCMI_CLOCK_EXTENDED_NAME_LENGTH_MAX];
};

/*
 * Clock notify on clock rate change
 */

struct scmi_clock_rate_notify_a2p {
    uint32_t clock_id;
    uint32_t notify_enable;
};

struct scmi_clock_rate_notify_p2a {
    int32_t status;
};

/*
 * Clock notify on clock rate change requests
 */

struct scmi_clock_rate_change_request_notify_a2p {
    uint32_t clock_id;
    uint32_t notify_enable;
};

struct scmi_clock_rate_change_request_notify_p2a {
    int32_t status;
};

/*
 * Clock get duty cycle
 */

struct scmi_clock_duty_cycle_get_a2p {
    uint32_t clock_id;
};

struct scmi_clock_duty_cycle_get_p2a {
    int32_t status;
    uint32_t numerator;
    uint32_t denominator;
};

/*
 * Clock get round rate
 */

struct scmi_clock_round_rate_get_a2p {
    uint32_t flags;
    uint32_t clock_id;
    uint32_t rate[2];
};

struct scmi_clock_round_rate_get_p2a {
    int32_t status;
    uint32_t rate[2];
};

/*
 * Enabling and disabling a clock
 */
static int scmi_clk_gate(struct scmi_channel *channel, unsigned int clock_id, int enable)
{
	struct scmi_clock_config_set_p2a response = { };
	struct scmi_clock_config_set_a2p message = {
		.clock_id = clock_id,
		.attributes = SCMI_CLOCK_CONFIG_SET_ENABLE(enable),
	};
	struct scmi_message_data a2p = {
		.channel = channel,
		.protocol_id = SCMI_PROTOCOL_ID_CLOCK,
		.message_id = SCMI_CLOCK_CONFIG_SET,
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

/* Function to register to clock framework */
int scmi_clock_enable(struct scmi_clock *clock)
{
	//struct scmi_clock *scmi_clock = clock->private;

	return scmi_clk_gate(clock->channel, clock->clock_id, 1);
}

/* Function to register to clock framework */
int scmi_clock_disable(struct scmi_clock *clock)
{
	//struct scmi_clock *scmi_clock = clock->private;

	return scmi_clk_gate(clock->channel, clock->clock_id, 0);
}

#if 0
/*
 * Get and set a clock rate
 */

/* Function to register to clock framework */
int scmi_clock_get_rate(struct scmi_clock *clock, unsigned int *rate_khz)
{
	//struct scmi_clock *scmi_clock = clock->private;
	struct scmi_clk_rate_get_p2a response = { };
	struct scmi_clk_rate_get_a2p message = {
		.clock_id = clock->clock_id,
	};
	struct scmi_message_data a2p = {
		.channel = channel,
		.protocol_id = SCMI_PROTOCOL_ID_CLOCK,
		.message_id = SCMI_CLOCK_RATE_GET,
		.message = &message,
		.message_size = sizeof(message),
		.response = &response,
		.response_size = sizeof(response),
	};
	uint64_t rate_hz = 0;
	int ret = 0;

	ret = scmi_process_message(&a2p);
	if (ret)
		return ret;

	ret = scmi_status_to_ret(response.status);
	if (ret)
		return ret;

	rate_hz = ((uint64_t)response.out.rate.msb << 32) | out.rate.lsb;
	*rate_khz = rate_hz / 10;

	return 0;
}

/* Function to register to clock framework */
int scmi_clock_set_rate(struct clock *clock, unsigned int rate_khz)
{
	struct scmi_clock *scmi_clock = clock->private;
	struct scmi_clk_rate_set_p2a response = { };
	struct scmi_clk_rate_set_a2p message = {
		.clock_id = scmi_clock->clock_id,
		/* Here we assume the request is synchronous (blocking) */
		.flags = SCMI_CLOCK_RATE_SET_ASYNC(0) |
			 SCMI_CLOCK_RATE_SET_NO_DELAYED_RESPONSE(0) |
			 SCMI_CLK_RATE_ROUND_CLOSEST,

		.rate.lsb = (uint32_t)(rate_khz * 1000);
		.rate.msb = (uint32_t)(((uint64_t)rate_khz * 1000) >> 32);

	};
	struct scmi_message_data message = {
		.channel = channel,
		.protocol_id = SCMI_PROTOCOL_ID_CLOCK,
		.message_id = SCMI_CLOCK_RATE_SET,
		.message = &message,
		.message_size = sizeof(message),
		.response = &response,
		.response_size = sizeof(response),
	};
	int ret = 0;

	ret = scmi_process_msg(&message);
	if (ret) {
		return ret;
	}

	return scmi_status_to_ret(response.status);
}

/*
 * Initialize a clock structure. Optional.
 * This function allows to check the target SCMI clock exists and has a name.
 * It is not optimized has is queries clock protocol information for each
 * clock whereas those could be retrived once.
 */

/* Return the number of clocks exposed or a negative value on error */
static int scmi_get_clock_count(struct scmi_channel *channel)
{
	struct scmi_clk_rate_set_p2a response = { };
	struct scmi_message_data message = {
		.channel = channel,
		.protocol_id = SCMI_PROTOCOL_ID_CLOCK,
		.message_id = SCMI_CLOCK_PROTO_ATTRIBS,
		.response = &response,
		.response_size = sizeof(response),
	};
	int ret = 0;

	ret = scmi_process_msg(&message);
	if (ret)
		goto err;

	ret = scmi_status_to_ret(response.status);
	if (ret)
		goto err;

	return SCMI_CLOCK_PROTO_ATTRIBS_CLOCK_COUNT(response.attributes);

err:
	assert(ret < 0);
	return ret;
}

char *scmi_get_clock_name(struct scmi_clock *scmi_clock)
{
	return "\0";
}

/* Function to possibly register to the clock framework */
int scmi_clock_init(struct clock *clock)
{
	struct scmi_clock *scmi_clock = clock->private;

	struct scmi_clock *scmi_clock = clock->private;
	struct scmi_clk_rate_set_p2a response = { };
	struct scmi_clk_rate_set_a2p message = {
		.clock_id = scmi_clock->clock_id,
		/* Here we assume the request is syncrhonous (blocking) */
		.flags = SCMI_CLOCK_RATE_SET_ASYNC(0) |
			 SCMI_CLOCK_RATE_SET_NO_DELAYED_RESPONSE(0) |
			 SCMI_CLK_RATE_ROUND_CLOSEST,

		.rate.lsb = (uint32_t)(rate_khz * 1000);
		.rate.msb = (uint32_t)(((uint64_t)rate_khz * 1000) >> 32);

	};
	struct scmi_message_data message = {
		.channel = channel,
		.protocol_id = SCMI_PROTOCOL_ID_CLOCK,
		.message_id = CLOCK_RATE_SET,
		.message = &message,
		.message_size = sizeof(message),
		.response = &response,
		.response_size = sizeof(response),
	};
	int ret = 0;

	ret = scmi_process_msg(&message);
	if (ret)
		return ret;

	return scmi_status_to_ret(response.status);
}
#endif
