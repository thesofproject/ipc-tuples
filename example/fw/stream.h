/* SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright(c) 2020 Intel Corporation. All rights reserved.
 *
 * Author: Liam Girdwood <liam.r.girdwood@linux.intel.com>
 */

/*
 * This file is an example of an existing API header in firmware modified for
 * IPCT.
 *
 * This file is completely private to the FW and interfaces to the SW drivers
 * running on the host. Changes to internal structure and limits here
 * have no effect on ABI regardless of of any differences in SW or FW versions.
 */


#ifndef EXAMPLE_FW_STREAM_H_
#define EXAMPLE_FW_STREAM_H_

#include <stdint.h>
#include <stdlib.h>

/* constants used for tuple value validation */

#define STREAM_ID_MIN			0
#define STREAM_ID_MAX			255

#define STREAM_CHANNEL_MIN		1
#define STREAM_CHANNEL_MAX		8

#define STREAM_RATE_MIN			8000
#define STREAM_RATE_MAX			192000

#define STREAM_BYTES_VALID_MIN		2
#define STREAM_BYTES_VALID_MAX		4

#define STREAM_BYTES_CONTAINER_MIN	2
#define STREAM_BYTES_CONTAINER_MAX	4

#define STREAM_CHMAP_COLOR_SIZE		8
#define STREAM_CHMAP_NAME_SIZE		4

/**
 * @brief Stream general purpose message context.
 *
 * Data need to identify a stream
 *
 * @ipc action=STREAM_ACTION_POSITION
 * @ipc member=id, token=STREAM_POSN_ID, min=STREAM_ID_MIN, max=STREAM_ID_MAX
 * @ipc action=STREAM_ACTION_EXAMPLE1
 * @ipc member=id, token=STREAM_EXAMPLE_ID, min=STREAM_ID_MIN, max=STREAM_ID_MAX
 */
struct stream_message {
	uint32_t id;		/**< The stream ID */
};

/**
 * @brief Stream trigger context.
 *
 * Data need to trigger a stream.
 *
 * @ipc action=STREAM_ACTION_TRIGGER
 * @ipc member=id, token=STREAM_TRIGGER_ID, min=STREAM_ID_MIN, max=STREAM_ID_MAX
 * @ipc member=trigger_cmd, token=STREAM_TRIGGER_CMD,enum=stream_trigger_cmd
 */
struct stream_trigger {
	uint32_t id;		/**< The stream ID */
	uint32_t trigger_cmd;	/**< Trigger command */
};

/**
 * @brief Stream position context.
 *
 * Stream position at DAI and host sides of pipeline.
 *
 * @ipc action=STREAM_ACTION_POSITION_REPLY
 * @ipc member=id, token=STREAM_POSN_ID,min=STREAM_ID_MIN, max=STREAM_ID_MAX
 * @ipc member=host, token=STREAM_POSN_DAI
 * @ipc member=dai, token=STREAM_POSN_HOST
 */
struct stream_position {
	uint32_t id;	/**< The stream ID */
	uint32_t host;	/**< Stream position at host buffer */
	uint32_t dai;	/**< Stream position at DAI buffer */
};

/**
 * @brief Stream "color" (EXAMPLE OF SUB STRUCTs).
 *
 * private stream structure - can be changed with no ABI breakage
 * a meaningless structure intended to show nested array processing
 *
 * @ipc subaction=STREAM_ACTION_PARAMS
 * @ipc member=a, token=STREAM_PARAMS_CHMAP_COLOR_A
 * @ipc member=b, token=STREAM_PARAMS_CHMAP_COLOR_B
 */
struct stream_color {
	uint32_t a;
	uint32_t b;
};

/**
 * @brief Stream channel mapping (EXAMPLE OF SUB STRUCTs).
 *
 * private stream structure - can be changed with no ABI breakage
 * a meaningless structure intended to show nested array processing
 *
 * @ipc subaction=STREAM_ACTION_PARAMS
 * @ipc member=id, token=STREAM_PARAMS_CHMAP_ID, min=STREAM_CHANNEL_MIN, max=STREAM_CHANNEL_MAX
 * @ipc member=name, token=STREAM_PARAMS_CHMAP_NAME, elems=STREAM_CHMAP_NAME_SIZE
 * @ipc member=color, elems=STREAM_CHMAP_COLOR_SIZE
 */
struct stream_chmap {
	uint32_t id;	/**< The channel ID */
	char name[STREAM_CHMAP_NAME_SIZE];
	struct stream_color color[STREAM_CHMAP_COLOR_SIZE]; /* meaningless - shows nested arrays */
};

/**
 * @brief Stream runtime parameters.
 *
 * private stream structure - can be changed with no ABI breakage
 * a meaningless structure intended to show nested array processing
 *
 * @ipc action=STREAM_ACTION_PARAMS
 * @ipc member=id, token=STREAM_PARAMS_ID, min=STREAM_ID_MIN, max=STREAM_ID_MAX
 * @ipc member=direction, token=STREAM_PARAMS_DIRECTION, enum sof_ipc_stream_direction
 * @ipc member=frame_fmt, token=STREAM_PARAMS_FRAME_FMT, enum stream_frame_format
 * @ipc member=buffer_fmt, token=STREAM_PARAMS_BUFFER_FMT, enum sof_ipc_buffer_format
 * @ipc member=rate, token=STREAM_PARAMS_RATE, min=STREAM_RATE_MIN, max=STREAM_RATE_MAX
 * @ipc member=stream_tag, token=STREAM_PARAMS_STREAM_TAG
 * @ipc member=channels, token=STREAM_PARAMS_CHANNELS, min=STREAM_CHANNEL_MIN, max=STREAM_CHANNEL_MAX
 * @ipc member=sample_valid_bytes, token=STREAM_PARAMS_SAMPLE_VALID_BYTES, min=STREAM_BYTES_VALID_MIN, max=STREAM_BYTES_VALID_MAX
 * @ipc member=sample_container_bytes, token=STREAM_PARAMS_SAMPLE_CONTAINER_BYTES, min=STREAM_BYTES_CONTAINER_MIN, max=STREAM_BYTES_CONTAINER_MAX
 * @ipc member=host_period_bytes token=STREAM_PARAMS_HOST_PERIOD_BYTES
 * @ipc member=stream_no_irq, token=STREAM_PARAMS_STREAM_NO_IRQ, boolean, optional
 * @ipc member=chmap, elems=STREAM_CHANNEL_MAX
 */
struct stream_params {
	uint32_t id;		/**< The stream ID */
	uint32_t direction;	/**< The stream direction */
	uint32_t frame_fmt;	/**< The stream frame format */
	uint32_t buffer_fmt;	/**< The stream buffer format */
	uint32_t rate;		/**< The stream sample rate */
	uint16_t stream_tag;	/**< The stream tag for DMA */
	uint16_t channels;	/**< The number of channels */
	uint16_t sample_valid_bytes;	/**< Sample size in bytes */
	uint16_t sample_container_bytes;	/**< Sample container size in bytes */

	uint32_t host_period_bytes;	/**< The host period size */
	uint16_t stream_no_irq; /**< flag for no FW IRQs to host */

	/* nested and array structures also handled by ipc.*/
	struct stream_chmap chmap[STREAM_CHANNEL_MAX];
};

/**
 * @brief Stream channel mapping (EXAMPLE OF SUB STRUCTs).
 *
 * private stream structure - can be changed with no ABI breakage
 * a meaningless structure intended to show nested array processing
 *
 * @ipc subaction=STREAM_ACTION_PARAMS_REPLY
 * @ipc member=id, token=STREAM_PARAMS_REPLY_ID, min=STREAM_ID_MIN, max=STREAM_ID_MAX
 * @ipc member=stream_position_offset, token=STREAM_PARAMS_REPLY_POSN_OFFSET
 */
struct stream_params_reply {
	uint32_t id;		/**< The stream ID */
	uint32_t stream_position_offset;	/**< The stream position offest */
};

/*
 * Example Internal FW stream APIs.
 */
struct stream_context;

int stream_trigger(struct stream_context *ctx, struct stream_trigger *trig);

int stream_params(struct stream_context *ctx, struct stream_params *params);

int stream_init_subsystem(void);

#endif
