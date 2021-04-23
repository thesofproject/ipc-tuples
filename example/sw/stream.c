/* SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright(c) 2020 Intel Corporation. All rights reserved.
 *
 * Author: Liam Girdwood <liam.r.girdwood@linux.intel.com>
 */

#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include <string.h>

/* SW shared tuples with IO peers - shared with FW and SW IPC peers */
#include <shared/stream.h>
#include <shared/classes.h>

/* IPCT infrastructure */
#include <ipct/client.h>

/* local headers*/
#include "test.h"
#include "stream.h"

#define STREAM_ACTION(x) \
	IPCT_ACTION_ID(IPCT_CLASS_AUDIO, IPCT_SUBCLASS_AUDIO_STREAM, x)

/* FAKE example driver context */
struct alsa_stream_context {
	int device;
	int stream_id;
	void *that;
	void *pcm;
};

static struct alsa_stream_context alsa_ctx;

/* FAKE mailbox to emulate real HW */
#define MAILBOX_SIZE 	1024
static char mailbox[MAILBOX_SIZE];

static int sw_stream_params(struct alsa_stream_context *ctx)
{
	struct alsa_hw_params params;
	int size;

	/* set stream params */
	memset(&params, 0, sizeof(params));
	params.rate = 44100;
	params.channels = 2;
	params.host_period_bytes = 256;
	params.stream_tag = 7;
	params.direction = stream_direction_capture;
	params.frame_fmt = stream_frame_format_s32_le;
	params.sample_container_bytes = 4;
	params.sample_valid_bytes = 4;
	params.id = 20;

	/* reply to the host with stream information */
	size = ipct_msg_pack(STREAM_ACTION(STREAM_ACTION_PARAMS), &params, sizeof(params),
			    mailbox, MAILBOX_SIZE, IPCT_FLAGS_REPLY_ACK, 0);
	if (size < 0) {
		fprintf(stderr, "error: failed to pack stream params\n");
		return size;
	}

	/* send message to FW */
	test_send_ipc_to_fw(mailbox, size);

	return 0;
}

/* send trigger command to FW */
static int sw_stream_trigger(struct alsa_stream_context *ctx, enum stream_trigger_cmd cmd)
{
	struct alsa_stream_trigger trigger;
	uint32_t flags = 0, addr = 0;
	int size;

	/* set trigger config */
	trigger.trigger_cmd = cmd;
	trigger.id = 20;

	size = ipct_msg_pack(STREAM_ACTION(STREAM_ACTION_TRIGGER),
			    &trigger, sizeof(trigger), mailbox, MAILBOX_SIZE,
			    flags, addr);
	if (size < 0) {
		fprintf(stderr, "error: failed to pack stream trigger\n");
		return size;
	}

	/* send message to FW */
	test_send_ipc_to_fw(mailbox, size);

	return 0;
}

/* get stream position from FW */
static int sw_stream_position(struct alsa_stream_context *ctx)
{
	struct alsa_stream_message msg;
	int size;

	/* set message */
	msg.id = 20;

	/* reply to the host with stream information */
	size = ipct_msg_pack(STREAM_ACTION_POSITION, &msg, sizeof(msg),
			    mailbox, MAILBOX_SIZE, IPCT_FLAGS_PRIORTY, 0);
	if (size < 0) {
		fprintf(stderr, "error: failed to pack stream position\n");
		return size;
	}

	/* send message to FW */
	test_send_ipc_to_fw(mailbox, size);

	return 0;
}

/* stream position returned from FW */
static int sw_stream_position_reply(struct alsa_stream_context *ctx,
			      struct alsa_stream_position *posn)
{
	fprintf(stdout, "posn id %d host %d dai %d\n", posn->id,
		posn->host, posn->dai);
	return 0;
}

/* subclass ACTION handler */
int sw_stream_ipc_handler(uint32_t id, void *structure, uint32_t size)
{

	switch (id) {
	case STREAM_ACTION(STREAM_ACTION_POSITION_REPLY):
		return sw_stream_position_reply(&alsa_ctx, structure);
	default:
		/* to remind developers they are missing an action  */
		fprintf(stderr, "error: stream action %x not implemented\n", id);
		return -EINVAL;
	}

	return 0;
}

int sw_run_stream_tests(void)
{
	int ret;

	ret = sw_stream_params(&alsa_ctx);
	if (ret < 0) {
		fprintf(stderr, "error: alsa failed to set stream params\n");
		return ret;
	}

	ret = sw_stream_trigger(&alsa_ctx, stream_trigger_start);
	if (ret < 0) {
		fprintf(stderr, "error: alsa failed to set stream params\n");
		return ret;
	}

	ret = sw_stream_position(&alsa_ctx);
	if (ret < 0) {
		fprintf(stderr, "error: alsa failed to set stream params\n");
		return ret;
	}

	ret = sw_stream_trigger(&alsa_ctx, stream_trigger_stop);
	if (ret < 0) {
		fprintf(stderr, "error: alsa failed to set stream params\n");
		return ret;
	}

	return 0;
}
