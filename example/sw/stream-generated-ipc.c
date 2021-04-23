/* SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright(c) 2020 Intel Corporation. All rights reserved.
 *
 * DO NOT EDIT - THIS FILE (WILL BE) AUTOMATICALLY GENERATED.
 */

/* common framework */
#include <shared/stream.h>
#include <shared/classes.h>
#include <ipct/builder.h>

/* local header */
#include "stream.h"

/**
 * Generated
 *
 * private stream structure - can be changed with no ABI breakage
 * a meaningless structure intended to show nested array processing
 *
 * ipc: action=ALSA_STREAM_ACTION_PARAMS
 * ipc: member=id, min=ALSA_STREAM_ID_MIN, max=ALSA_STREAM_ID_MAX
 * ipc: member=direction, enum sof_ipc_stream_direction
 * ipc: member=frame_fmt, enum stream_frame_format
 * ipc: member=buffer_fmt, enum sof_ipc_buffer_format
 * ipc: member=rate, min=ALSA_STREAM_RATE_MIN, max=ALSA_STREAM_RATE_MAX
 * ipc: member=stream_tag
 * ipc: member=channels, min=ALSA_STREAM_CHANNEL_MIN, max=ALSA_STREAM_CHANNEL_MAX
 * ipc: member=sample_valid_bytes, min=ALSA_STREAM_BYTES_VALID_MIN, max=ALSA_STREAM_BYTES_VALID_MAX
 * ipc: member=sample_container_bytes, min=ALSA_STREAM_BYTES_CONTAINER_MIN, max=ALSA_STREAM_BYTES_CONTAINER_MAX
 * ipc: member=host_period_bytes
 * ipc: member=stream_no_irq, boolean, optional
 * ipc: member=chmap, elems=ALSA_STREAM_CHANNEL_MAX
 */

/* mandatory tuples - struct alsa_hw_params */
IPCT_DECLARE_TUPLE_ELEMS(stream_config_man,
	IPCT_TUPLE_ELEM(STREAM_PARAMS_ID, ipct_type_uint32_value,
			offsetof(struct alsa_hw_params, id),
			ALSA_STREAM_ID_MIN, ALSA_STREAM_ID_MAX),
	IPCT_TUPLE_ELEM(STREAM_PARAMS_DIRECTION, ipct_type_enum,
			offsetof(struct alsa_hw_params, direction),
			0, 0),
	IPCT_TUPLE_ELEM(STREAM_PARAMS_FRAME_FMT, ipct_type_enum,
			offsetof(struct alsa_hw_params, frame_fmt),
			0, 0),
	IPCT_TUPLE_ELEM(STREAM_PARAMS_BUFFER_FMT, ipct_type_enum,
			offsetof(struct alsa_hw_params, buffer_fmt),
			0, 0),
	IPCT_TUPLE_ELEM(STREAM_PARAMS_RATE, ipct_type_uint32_value,
			offsetof(struct alsa_hw_params, rate),
			ALSA_STREAM_RATE_MIN, ALSA_STREAM_RATE_MAX),
	IPCT_TUPLE_ELEM(STREAM_PARAMS_STREAM_TAG, ipct_type_uint16_value,
			offsetof(struct alsa_hw_params, stream_tag),
			0, 0),
	IPCT_TUPLE_ELEM(STREAM_PARAMS_CHANNELS, ipct_type_uint16_value,
			offsetof(struct alsa_hw_params, channels),
			ALSA_STREAM_CHANNEL_MIN, ALSA_STREAM_CHANNEL_MAX),
	IPCT_TUPLE_ELEM(STREAM_PARAMS_SAMPLE_VALID_BYTES, ipct_type_uint16_value,
			offsetof(struct alsa_hw_params, sample_valid_bytes),
			ALSA_STREAM_BYTES_VALID_MIN, ALSA_STREAM_BYTES_VALID_MAX),
	IPCT_TUPLE_ELEM(STREAM_PARAMS_SAMPLE_CONTAINER_BYTES, ipct_type_uint16_value,
			offsetof(struct alsa_hw_params, sample_container_bytes),
			ALSA_STREAM_BYTES_CONTAINER_MIN, ALSA_STREAM_BYTES_CONTAINER_MAX),
	IPCT_TUPLE_ELEM(STREAM_PARAMS_HOST_PERIOD_BYTES, ipct_type_uint32_value,
			offsetof(struct alsa_hw_params, host_period_bytes),
			0, 0),
);

/* optional tuples - struct alsa_hw_params */
IPCT_DECLARE_TUPLE_ELEMS(stream_config_opt,
	IPCT_TUPLE_ELEM(STREAM_PARAMS_STREAM_NO_IRQ, ipct_type_boolean,
			offsetof(struct alsa_hw_params, stream_no_irq),
			0, 0),
);

/**
 * @brief Stream "color" (EXAMPLE OF SUB STRUCTs).
 *
 * private stream structure - can be changed with no ABI breakage
 * a meaningless structure intended to show nested array processing
 *
 * ipc: subaction=ALSA_STREAM_ACTION_PARAMS
 * ipc: member=a
 * ipc: member=b
 */
IPCT_DECLARE_TUPLE_ELEMS(stream_color_man,
	IPCT_TUPLE_ELEM(STREAM_PARAMS_CHMAP_ID, ipct_type_uint32_value,
			offsetof(struct alsa_stream_color, a),
			0, 32),
	IPCT_TUPLE_ELEM(STREAM_PARAMS_CHMAP_NAME, ipct_type_uint32_value,
			offsetof(struct alsa_stream_color, b),
			0, 32),
);

/* mandatory tuples - struct alsa_hw_params */
/**
 * @brief Stream channel mapping (EXAMPLE OF SUB STRUCTs).
 *
 * private stream structure - can be changed with no ABI breakage
 * a meaningless structure intended to show nested array processing
 *
 * ipc: subaction=ALSA_STREAM_ACTION_PARAMS
 * ipc: member=id, min=ALSA_STREAM_CHANNEL_MIN, max=ALSA_STREAM_CHANNEL_MAX
 * ipc: member=name, elems=ALSA_STREAM_CHMAP_NAME_SIZE
 * ipc: member=color, elems=ALSA_STREAM_CHMAP_COLOR_SIZE
 */
IPCT_DECLARE_TUPLE_ELEMS(stream_chmap_man,
	IPCT_TUPLE_ELEM(STREAM_PARAMS_CHMAP_ID, ipct_type_uint32_value,
			offsetof(struct alsa_stream_chmap, id),
			ALSA_STREAM_ID_MIN, ALSA_STREAM_ID_MAX),
	IPCT_TUPLE_ELEM(STREAM_PARAMS_CHMAP_NAME, ipct_type_string,
			offsetof(struct alsa_stream_chmap, name),
			ALSA_STREAM_CHMAP_NAME_SIZE, 0),
);

/* descriptors for child structures of stream_params */
IPCT_DECLARE_SUBACTION_DESC(alsa_hw_params,
	IPCT_DECLARE_SUBACTION_ELEM(chmap, alsa_hw_params,
			IPCT_TUPLES(stream_chmap_man),
			IPCT_NOTUPLES,
			0, IPCT_NOSUBACTION));

/* descriptor for struct alsa_hw_params */
IPCT_DECLARE_ACTION_DESC(alsa_hw_params,
		IPCT_TUPLES(stream_config_man),
		IPCT_TUPLES(stream_config_opt),
		0, IPCT_SUBACTION(alsa_hw_params));



/**
 * @brief Stream trigger context.
 *
 * Data need to trigger a stream.
 *
 * @ipc action=ALSA_STREAM_ACTION_TRIGGER
 * @ipc member=id, token=ALSA_STREAM_TRIGGER_ID, min=ALSA_STREAM_ID_MIN, max=ALSA_STREAM_ID_MAX
 * @ipc member=trigger_cmd, token=ALSA_STREAM_TRIGGER_CMD,enum=stream_trigger_cmd
 */

/* mandatory tuples - struct alsa_hw_params */
IPCT_DECLARE_TUPLE_ELEMS(stream_trigger_man,
	IPCT_TUPLE_ELEM(STREAM_TRIGGER_ID, ipct_type_uint32_value,
			offsetof(struct alsa_stream_trigger, id),
			ALSA_STREAM_ID_MIN, ALSA_STREAM_ID_MAX),
	IPCT_TUPLE_ELEM(STREAM_TRIGGER_CMD, ipct_type_enum,
			offsetof(struct alsa_stream_trigger, trigger_cmd),
			STREAM_TRIGGER_CMD_ENUM_MIN, STREAM_TRIGGER_CMD_ENUM_MAX),
);

/* descriptor for struct alsa_stream_trigger */
IPCT_DECLARE_ACTION_DESC(alsa_stream_trigger,
		IPCT_TUPLES(stream_trigger_man),
		IPCT_NOTUPLES,
		0, IPCT_NOSUBACTION);

IPCT_DECLARE_ACTIONS(stream,
		IPCT_ACTION(STREAM_ACTION_PARAMS, alsa_hw_params),
		IPCT_ACTION(STREAM_ACTION_TRIGGER, alsa_stream_trigger),
);

/* Declare subclass IPCT_SUBCLASS_AUDIO_STREAM */
IPCT_DECLARE_SUBCLASS(stream, IPCT_SUBCLASS_AUDIO_STREAM, stream_actions);

/* TODO - generated */
struct ipct_klass_def builder_klass = {
	.klass_id	= IPCT_CLASS_AUDIO,
	.num_subklasses	= 1,
	.subklass	= &stream_subclass,
};

struct ipct_klass_list builder_klasses = {
	.num_klasses = 1,
	.klasses = &builder_klass,
};
