// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright (c) 2019-2024, STMicroelectronics
 */

#ifndef SCMI_H
#define SCMI_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "stm32mp2xx_hal.h"

/*
#define CONFIG_64BIT
*/

#ifdef CONFIG_64BIT
#define BITS_PER_LONG  64
#else  /* CONFIG_64BIT */
#define BITS_PER_LONG  32
#endif /* CONFIG_64BIT */

#ifndef BIT
#define BIT(nr)         (1UL << (nr))
#endif /* BIT */

/*
 * Create a contiguous bitmask starting at bit position @l and ending at
 * position @h. For example
 * GENMASK_ULL(39, 21) gives us the 64bit vector 0x000000ffffe00000.
 */
#define GENMASK(h, l) \
	(((~0) << (l)) & (~0 >> (BITS_PER_LONG - 1 - (h))))

#define SCMI_SHM_START_ADDRESS    0x81200000
#define SCMI_SHM_SIZE             0x100000
#define SCMI_TIMEOUT_MS           1000

/* Identifiers for SCMI protocols */
enum scmi_protocol_id {
	SCMI_PROTOCOL_ID_BASE = 0x10,
	SCMI_PROTOCOL_ID_POWER_DOMAIN = 0x11,
	SCMI_PROTOCOL_ID_SYSTEM = 0x12,
	SCMI_PROTOCOL_ID_PERF = 0x13,
	SCMI_PROTOCOL_ID_CLOCK = 0x14,
	SCMI_PROTOCOL_ID_SENSOR = 0x15,
	SCMI_PROTOCOL_ID_RESET_DOMAIN = 0x16,
	SCMI_PROTOCOL_ID_VOLTAGE_DOMAIN = 0x17,
};

/* Identifiers for SCMI status in response payloads */
enum scmi_status_code {
	SCMI_SUCCESS =  0,
	SCMI_NOT_SUPPORTED = -1,
	SCMI_INVALID_PARAMETERS = -2,
	SCMI_DENIED = -3,
	SCMI_NOT_FOUND = -4,
	SCMI_OUT_OF_RANGE = -5,
	SCMI_BUSY = -6,
	SCMI_COMMS_ERROR = -7,
	SCMI_GENERIC_ERROR = -8,
	SCMI_HARDWARE_ERROR = -9,
	SCMI_PROTOCOL_ERROR = -10,
};

/* SCMI message IDs common to all protocols */
enum scmi_common_message_id {
	SCMI_PROTOCOL_VERSION = 0,
	SCMI_PROTOCOL_ATTRIBUTES = 1,
	SCMI_PROTOCOL_MESSAGE_ATTRIBUTES = 2
};

/* Opaque SCMI channel which content may depend on the SCMI transport */
struct scmi_channel;

/*
 * struct scmi_message_data - SCMI message and response
 *
 * @channel: SCMI channel for the communication
 * @protocol_id: SCMI protocol ID for the message
 * @message_id: SCMI message ID sent on @protocol
 * @message: Reference to the SCMI message payload to send
 * @message_size: Byte size of @message
 * @response: Buffer for the SCMI response payload
 * @response_size: Byte size of @response
 */
struct scmi_message_data {
	struct scmi_channel *channel;
	uint32_t protocol_id;
	uint32_t message_id;
	void *message;
	size_t message_size;
	void *response;
	size_t response_size;
};

/*
 * struct scmi_channel - An SCMI channel instance
 * @tx_mailbox: Mailbox for client to server request
 * @tx_buffer: Shared 128byte buffer for client to server request
 * @rx_mailbox: Mailbox for client to server request
 * @rx_buffer: Shared 128byte buffer for client to server request
 */
struct scmi_channel {
	unsigned int tx_timeout_ms;
	uint32_t *rx_buffer;
	uint32_t *tx_buffer;
};

/**
 * scmi_process_message() - Send an SCMI message and get the response
 *
 * @message_data: Data for the requested SCMI communication
 * @return 0 on success, a negative errno otherwise
 */
int scmi_process_message(struct scmi_message_data *message_data);

/**
 * scmi_status_to_ret() - Convert an SCMI status code into host error code
 *
 * @scmi_status: SCMI status code value, expected in enum scmi_status_code
 * @return 0 on SCMI success and a meaningful non-zero error code otherwise
 */
int scmi_status_to_ret(int32_t scmi_status);

int scmi_voltage_domain_enable(struct scmi_channel *channel, unsigned int domain_id);
int scmi_voltage_domain_disable(struct scmi_channel *channel, unsigned int domain_id);

#endif /* SCMI_H */

