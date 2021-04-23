/* SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright(c) 2020 Intel Corporation. All rights reserved.
 *
 * Author: Liam Girdwood <liam.r.girdwood@linux.intel.com>
 */

#include <stdio.h>
#include <errno.h>
#include <assert.h>

/* FW shared tuples with IO peers - shared with SW and FW IPC peers */
#include <shared/stream.h>
#include <shared/classes.h>

/* IPCT infrastructure */
#include <ipct/client.h>

/* local headers*/
#include "test.h"
#include "stream.h"

#define STREAM_ACTION(x) \
	IPCT_ACTION_ID(IPCT_CLASS_AUDIO, IPCT_SUBCLASS_AUDIO_STREAM, x)

/* example firmware context */
struct fw_stream_context {
	int blah;
	void *that;
	void *parent;
};

/* our FAKE test context */
static struct fw_stream_context ctx;

/* FAKE mailbox to emulate real HW */
#define MAILBOX_SIZE 	1024
static char mailbox[MAILBOX_SIZE];


static int fw_stream_params(struct fw_stream_context *ctx,
			 struct stream_params *params)
{
	struct stream_params_reply reply;
	int ret;

	fprintf(stdout, "params: id  %d rate %d bytes %d\n", params->id,
		params->rate, params->sample_container_bytes);

	reply.id = params->id;
	reply.stream_position_offset = 0x10;

	/* reply to the host with stream information */
	ret = ipct_msg_pack(STREAM_ACTION(STREAM_ACTION_PARAMS_REPLY),
			    &reply, sizeof(reply),
			    mailbox, MAILBOX_SIZE, IPCT_FLAGS_REPLY_ACK, 0);

	return ret;
}

static int fw_stream_trigger(struct fw_stream_context *ctx, struct stream_trigger *trig)
{
	enum stream_trigger_cmd cmd = trig->trigger_cmd;

	fprintf(stdout, "fw: trigger %d\n", cmd);

	switch (cmd) {
	case stream_trigger_start:
		/* do stream start */
		break;
	case stream_trigger_stop:
		/* do stream stop */
		break;
	case stream_trigger_pause:
		/* do stream pause */
		break;
	case stream_trigger_release:
		/* do stream release */
		break;
	default:
		fprintf(stderr, "error: unknown trigger cmd %d\n", cmd);
	}
}

static int fw_stream_position(struct fw_stream_context *ctx,
		struct stream_message *msg)
{
	struct stream_position posn;
	int size;

	/* fake posn */
	posn.dai = 10;
	posn.host = 5;
	posn.id = msg->id;

	/* reply to the host with stream information */
	size = ipct_msg_pack(STREAM_ACTION(STREAM_ACTION_POSITION_REPLY),
			     &posn, sizeof(posn),
			     mailbox, MAILBOX_SIZE, IPCT_FLAGS_PRIORTY, 0);
	if (size < 0) {
		fprintf(stderr, "error: failed to pack stream position\n");
		return size;
	}

	/* send message to FW */
	test_send_ipc_to_fw(mailbox, size);

	return 0;
}

/* subclass ACTION handler */
int fw_stream_ipc_handler(uint32_t id, void *structure, uint32_t size)
{
	switch (id) {
	case STREAM_ACTION(STREAM_ACTION_PARAMS):
		return fw_stream_params(&ctx, structure);
	case STREAM_ACTION(STREAM_ACTION_TRIGGER):
		return fw_stream_trigger(&ctx, structure);
	case STREAM_ACTION(STREAM_ACTION_POSITION):
		return fw_stream_position(&ctx, structure);
	default:
		/* to remind developers they are missing an action  */
		fprintf(stderr, "error: stream action %x not implemented\n", id);
		return -EINVAL;
	}
}

