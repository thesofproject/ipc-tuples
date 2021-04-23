/* SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright(c) 2020 Intel Corporation. All rights reserved.
 *
 * Author: Liam Girdwood <liam.r.girdwood@linux.intel.com>
 */

#ifndef SRC_INCLUDE_EXAMPLE_ALSA_STREAM_H_
#define SRC_INCLUDE_EXAMPLE_ALSA_STREAM_H_

#include <stdint.h>
#include <stdlib.h>

/* common action and enum IDs for stream usage */
#include "stream.h"

/* This file is completely private to the SW and interfaces to the FW
 * running on the DSP. Changes to internal structure and limits here
 * have no effect on ABI regardless of of any differences in SW or FW versions.
 */

/* constants used for tuple value validation */

#define ALSA_STREAM_ID_MIN			0
#define ALSA_STREAM_ID_MAX			64

#define ALSA_STREAM_CHANNEL_MIN		1
#define ALSA_STREAM_CHANNEL_MAX		8

#define ALSA_STREAM_RATE_MIN			4000
#define ALSA_STREAM_RATE_MAX			192000

#define ALSA_STREAM_BYTES_VALID_MIN		2
#define ALSA_STREAM_BYTES_VALID_MAX		4

#define ALSA_STREAM_BYTES_CONTAINER_MIN	2
#define ALSA_STREAM_BYTES_CONTAINER_MAX	4

#define ALSA_STREAM_CHMAP_COLOR_SIZE		8
#define ALSA_STREAM_CHMAP_NAME_SIZE		4

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
struct alsa_stream_message {
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
struct alsa_stream_trigger {
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
struct alsa_stream_position {
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
/* private stream structure - can be changed with no ABI breakage */
/* a meaningless structure intended to show nested array processing */
struct alsa_stream_color {
	uint32_t a;
	uint32_t b;
	uint32_t c;
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
/* private stream structure - can be changed with no ABI breakage */
struct alsa_stream_chmap {
	struct substream *substream;
	uint32_t id;
	char name[ALSA_STREAM_CHMAP_NAME_SIZE];
	struct alsa_stream_color color[ALSA_STREAM_CHMAP_COLOR_SIZE]; /* meaningless - shows nested arrays */
};

/* private FW structures - can be changed at will with NO ABI breakage */
struct alsa_hw_params {
	uint32_t id;
	uint32_t direction;	/**< enum sof_ipc_stream_direction */
	uint32_t frame_fmt;	/**< enum sof_ipc_frame */
	uint32_t buffer_fmt;	/**< enum sof_ipc_buffer_format */
	uint32_t rate;
	uint16_t stream_tag;
	uint16_t channels;
	uint16_t sample_valid_bytes;
	uint16_t sample_container_bytes;
	uint16_t stream_no_irq;

	uint32_t host_period_bytes;
	uint16_t no_stream_position; /**< 1 means don't send stream position */

	struct alsa_stream_chmap chmap[ALSA_STREAM_CHANNEL_MAX];	/**< channel map - SOF_CHMAP_ */
};

/*
 * Examples - just stubs but show how IPC code will call into core FW services.
 */
int stream_start(int id);
static inline int stream_stop(int id);
static inline int stream_config(struct alsa_hw_params *params);

#endif
