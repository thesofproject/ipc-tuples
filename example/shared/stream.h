/* SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright(c) 2020 Intel Corporation. All rights reserved.
 *
 * DO NOT EDIT - THIS FILE (WILL BE) AUTOMATICALLY GENERATED.
 */

#ifndef SRC_INCLUDE_EXAMPLE_STREAM_H_
#define SRC_INCLUDE_EXAMPLE_STREAM_H_

/*
 * This file shares Tuple ACTION IDs and enum IDs between SW and FW for stream
 * configuration and control between SW and FW. This is the only shared file
 * between SW and FW for stream usage.
 *
 * The data here is completely made up and should only be used as an example.
 */

/* Stream ACTIONs and Tuples - shared IDs between driver and firmware */

/* All ACTIONs supported by audio stream example */
#define STREAM_ACTION_PARAMS	 	0
#define STREAM_ACTION_PARAMS_REPLY 	1
#define STREAM_ACTION_TRIGGER		2
#define STREAM_ACTION_POSITION		3
#define STREAM_ACTION_POSITION_REPLY	4
#define STREAM_ACTION_EXAMPLE_SMALL	5
#define STREAM_ACTION_EXAMPLE_LARGE	6

enum stream_trigger_cmd {
	stream_trigger_start		= 0,
	stream_trigger_stop 		= 1,
	stream_trigger_pause		= 2,
	stream_trigger_release		= 3,
};
#define STREAM_TRIGGER_CMD_ENUM_MIN	stream_trigger_start
#define STREAM_TRIGGER_CMD_ENUM_MAX	stream_trigger_release

/* Stream IPC_AUDIO_STREAM_CONFIG tuples */
#define STREAM_PARAMS_ID		0
#define STREAM_PARAMS_DIRECTION		1

enum stream_direction {
	stream_direction_playback	= 0,
	stream_direction_capture	= 1,
};
#define STREAM_DIRECTION_ENUM_MIN	stream_direction_playback
#define STREAM_DIRECTION_ENUM_MAX	stream_direction_capture

#define STREAM_PARAMS_CHANNELS		2
#define STREAM_PARAMS_RATE		3
#define STREAM_PARAMS_FRAME_FMT		4

enum stream_frame_format {
	stream_frame_format_s16_le	= 0,
	stream_frame_format_s24_4le	= 1,
	stream_frame_format_s32_le	= 2,
	stream_frame_format_float	= 3,
};
#define STREAM_FRAME_FORMAT_ENUM_MIN	stream_frame_format_s16_le
#define STREAM_FRAME_FORMAT_ENUM_MAX	stream_frame_format_float

#define STREAM_PARAMS_BUFFER_FMT	5

enum stream_format {
	stream_buffer_format_interleaved	= 0,
};
#define STREAM_BUFFER_FORMAT_ENUM_MIN	stream_buffer_format_interleaved
#define STREAM_BUFFER_FORMAT_ENUM_MAX	stream_buffer_format_interleaved

#define STREAM_PARAMS_SAMPLE_VALID_BYTES	6
#define STREAM_PARAMS_SAMPLE_CONTAINER_BYTES	7
#define STREAM_PARAMS_HOST_PERIOD_BYTES		8
#define STREAM_PARAMS_STREAM_TAG	9
#define STREAM_PARAMS_STREAM_NO_IRQ	10

/* channel map is a small variable array where we can have ID per item */
#define STREAM_PARAMS_CHMAP_ID		11
#define STREAM_PARAMS_CHMAP_NAME	12
#define STREAM_PARAMS_CHMAP_COLOR_A	13
#define STREAM_PARAMS_CHMAP_COLOR_B	14


/* Stream IPC_AUDIO_STREAM_PARAMS_REPLY tuples */
#define STREAM_PARAMS_REPLY_ID		0
#define STREAM_PARAMS_REPLY_POSN_OFFSET	1

/* Stream STREAM_ACTION_TRIGGER tuples */
#define STREAM_TRIGGER_ID			10
#define STREAM_TRIGGER_CMD			11

/* Stream IPC_AUDIO_STREAM_POSITION tuples */
#define STREAM_POSN_ID			0
#define STREAM_POSN_DAI			1
#define STREAM_POSN_HOST		2

#endif
