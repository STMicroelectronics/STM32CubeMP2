// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright (c) 2019, STMicroelectronics
 * Copyright (c) 2015-2019, Arm Limited and Contributors. All rights reserved.
 */

#include "ipcc.h"
#include "scmi.h"

/*
 * Shared Memory based Transport (SMT) relies on a 128byte piece of
 * shared memory, aligned on 32bit words that is used to exchange
 * SCMI messages. The 28 first bytes of the memory are used for SMT
 * protocol meta data, exchaged between communication initiator and
 * target
 *
 * SMT uses a 28 byte header prior message payload to handle the state of
 * the communication channel realized by the shared memory area and
 * to define SCMI protocol information the payload relates to.
 */
struct scmi_smt_header {
	uint32_t reserved;
	uint32_t channel_status;
	uint32_t reserved1[2];
	uint32_t flags;
	uint32_t length;
	uint32_t msg_header;
	uint8_t msg_payload[];
};

/*
 * struct scmi_channel - An SCMI channel instance
 * @tx_mailbox: Mailbox for client to server request
 * @tx_buffer: Shared 128byte buffer for client to server request
 * @rx_mailbox: Mailbox for client to server request
 * @rx_buffer: Shared 128byte buffer for client to server request
 */
struct scmi_channel {
	struct mailbox *tx_mailbox;
	uint32_t *tx_buffer;
	unsigned int tx_timeout_ms;
	struct mailbox *rx_mailbox;
	uint32_t *rx_buffer;
};

#define SMT_SHARED_MEMORY_SIZE	128

#define SMT_CHANNEL_STATUS_FREE		BIT(0)
#define SMT_CHANNEL_STATUS_ERROR	BIT(1)

#define SMT_FLAGS_INTR_ENABLED		BIT(0)

#define SMT_MSG_HEADER_TOKEN(token)		(((token) << 18) & GENMASK(31, 18))
#define SMT_MSG_HEADER_PROTOCOL_ID(proto)	(((proto) << 10) & GENMASK(17, 10))
#define SMT_MSG_HEADER_MESSAGE_TYPE(type)	(((type) << 18) & GENMASK(9, 8))
#define SMT_MSG_HEADER_MESSAGE_ID(id)		((id) & GENMASK(7, 0))

static int write_msg_to_smt(struct scmi_message_data *msg)
{
	struct scmi_smt_header *hdr = (void *)msg->channel->tx_buffer;

	if ((!msg->message && msg->message_size) || (!msg->response && msg->response_size))
		return -1;

	if (!(hdr->channel_status & SMT_CHANNEL_STATUS_FREE))
		return -1;

	if ((sizeof(*hdr) + msg->message_size > SMT_SHARED_MEMORY_SIZE) ||
	    (sizeof(*hdr) + msg->message_size > SMT_SHARED_MEMORY_SIZE))
		return -1;

	/* Load message in shared memory */
	hdr->channel_status &= ~SMT_CHANNEL_STATUS_FREE;
	hdr->length = msg->message_size + sizeof(hdr->msg_header);
	hdr->msg_header = SMT_MSG_HEADER_TOKEN(0) |
			  SMT_MSG_HEADER_MESSAGE_TYPE(0) |
			  SMT_MSG_HEADER_PROTOCOL_ID(msg->protocol_id) |
			  SMT_MSG_HEADER_MESSAGE_ID(msg->message_id);

	memcpy(hdr->msg_payload, msg->message, msg->message_size);

	return 0;
}

static int read_resp_from_smt(uint32_t *shared_buffer, struct scmi_message_data *msg)
{
	struct scmi_smt_header *hdr = (struct scmi_smt_header *)shared_buffer;

	if (!(hdr->channel_status & SMT_CHANNEL_STATUS_FREE))
		return -1;

	if (hdr->channel_status & SMT_CHANNEL_STATUS_ERROR)
		return -1;

	if (hdr->length > msg->response_size + sizeof(hdr->msg_header))
		return -1;

	/* Get the SCMI response payload data */
	msg->response_size = hdr->length - sizeof(hdr->msg_header);
	memcpy(msg->response, hdr->msg_payload, msg->response_size);

	return 0;
}

static void clear_smt_channel(uint32_t *shared_buffer)
{
	struct scmi_smt_header *hdr = (struct scmi_smt_header *)shared_buffer;

	hdr->channel_status &= SMT_CHANNEL_STATUS_ERROR;
}

int scmi_process_message(struct scmi_message_data *msg)
{
	int ret = 0;

	ret = write_msg_to_smt(msg);
	if (ret)
		return ret;
/*
	ret = mailbox_send(msg->channel->tx_mailbox, msg->channel->tx_buffer);
	if (ret)
		goto out;

	ret = mailbox_receive(msg->channel->tx_mailbox, msg->channel->tx_buffer, msg->channel->tx_timeout_ms);
	if (ret)
		goto out;
*/

	IPCC_Req();

	ret = read_resp_from_smt(msg->channel->tx_buffer, msg);
out:
	clear_smt_channel(msg->channel->tx_buffer);

	return ret;
}

int scmi_status_to_ret(int32_t scmi_status)
{
	return scmi_status;
}
