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
#include "mbox_scmi_conf.h"

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

#define SMT_CHANNEL_STATUS_FREE            BIT(0)
#define SMT_CHANNEL_STATUS_ERROR           BIT(1)

#define SMT_FLAGS_INTR_ENABLED             BIT(0)

#define SMT_MSG_HEADER_TOKEN(token)        (((token) << 18) & GENMASK(31, 18))
#define SMT_MSG_HEADER_PROTOCOL_ID(proto)  (((proto) << 10) & GENMASK(17, 10))
#define SMT_MSG_HEADER_MESSAGE_TYPE(type)  (((type)  << 18) & GENMASK(9, 8))
#define SMT_MSG_HEADER_MESSAGE_ID(id)      ((id) & GENMASK(7, 0))

/*
 * scmiagent library return codes
 *
 * In the case of SCMIAGENT_SCMI_ERROR, the last two digits
 * represents the error code returned by the SCMI protocol.
 */
enum scmiagent_error
{
    SCMIAGENT_NO_ERROR                 =  0,
    SCMIAGENT_NOT_ALLOCATED            = -100,
    SCMIAGENT_NOTHING_TO_RECEIVE       = -200,
    SCMIAGENT_CHANNEL_BUSY             = -300,
    SCMIAGENT_OUT_OF_MEMORY            = -400,
    SCMIAGENT_CHANNEL_ERROR            = -500,
    SCMIAGENT_SCMI_ERROR               = -600,
};

/* Identifiers for SCMI protocols */
enum scmi_protocol_id
{
    SCMI_PROTOCOL_ID_BASE              = 0x10,
    SCMI_PROTOCOL_ID_POWER_DOMAIN      = 0x11,
    SCMI_PROTOCOL_ID_SYSTEM            = 0x12,
    SCMI_PROTOCOL_ID_PERF              = 0x13,
    SCMI_PROTOCOL_ID_CLOCK             = 0x14,
    SCMI_PROTOCOL_ID_SENSOR            = 0x15,
    SCMI_PROTOCOL_ID_RESET_DOMAIN      = 0x16,
    SCMI_PROTOCOL_ID_VOLTAGE_DOMAIN    = 0x17,
};

/* Identifiers for SCMI status in response payloads */
enum scmi_status_code
{
    SCMI_SUCCESS                       =  0,
    SCMI_NOT_SUPPORTED                 = -1,
    SCMI_INVALID_PARAMETERS            = -2,
    SCMI_DENIED                        = -3,
    SCMI_NOT_FOUND                     = -4,
    SCMI_OUT_OF_RANGE                  = -5,
    SCMI_BUSY                          = -6,
    SCMI_COMMS_ERROR                   = -7,
    SCMI_GENERIC_ERROR                 = -8,
    SCMI_HARDWARE_ERROR                = -9,
    SCMI_PROTOCOL_ERROR                = -10,
};

/* SCMI message IDs common to all protocols */
enum scmi_common_message_id
{
    SCMI_PROTOCOL_VERSION              = 0,
    SCMI_PROTOCOL_ATTRIBUTES           = 1,
    SCMI_PROTOCOL_MESSAGE_ATTRIBUTES   = 2,
};

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
struct scmi_message_data
{
    struct scmi_channel  *channel;
    uint32_t              protocol_id;
    uint32_t              message_id;
    void                 *message;
    size_t                message_size;
    void                 *response;
    size_t                response_size;
};

/*
 * struct scmi_channel - An SCMI channel instance
 * @tx_buffer: Shared 128byte buffer for client to server request
 * @rx_buffer: Shared 128byte buffer for client to server request
 */
struct scmi_channel
{
    uint32_t   *rx_buffer;
    uint32_t   *tx_buffer;
};

extern struct scmi_channel scmi_channel;

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

int scmi_voltage_domain_enable  (struct scmi_channel *channel, unsigned int domain_id);
int scmi_voltage_domain_disable (struct scmi_channel *channel, unsigned int domain_id);

#endif /* SCMI_H */

