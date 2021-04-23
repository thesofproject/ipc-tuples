/* SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright(c) 2020 Intel Corporation. All rights reserved.
 *
 * Author: Liam Girdwood <liam.r.girdwood@linux.intel.com>
 */

#include <stdint.h>
#include <errno.h>
#include <stdio.h>
#include <assert.h>

#include <ipct/client.h>
#include <ipct/builder.h>
#include "priv.h"

/*
 * Data extractors - gets data from C ctx->src.bases.
 */

static inline int ipc_data_get(void *dest, uint32_t const *data,
				      uint32_t const length,
				      uint32_t const max_length)
{
	if (length > max_length) {
		ipct_err("error: data too long %d for container %d\n",
			length, max_length);
		return -EINVAL;
	}

	/* copy data */
	memcpy(dest, data, length);

	/* zero pad any remaining space */
	memset(dest + length, 0, max_length - length);
	return 0;
}

static inline int ipc_data_uint64_get(uint64_t *dest, uint64_t const *data,
				      uint64_t const min, uint64_t const max)
{
	if (*data < min || *data > max) {
		ipct_err("error: uint64_t %lu out of range %lu..%lu\n",
			*data, min, max);
		return -EINVAL;
	}
	*dest = *data;
	return 0;
}

static inline int ipc_data_int64_get(int64_t *dest, int64_t const *data,
				      int64_t const min, int64_t const max)
{
	if (*data < min || *data > max) {
		ipct_err("error: int64_t %ld out of range %ld..%ld\n",
			*data, min, max);
		return -EINVAL;
	}
	*dest = *data;
	return 0;
}

static inline int ipc_data_uint32_get(uint32_t *dest, uint32_t const *data,
				      uint32_t const min, uint32_t const max)
{
	if (*data < min || *data > max) {
		ipct_err("error: uint32_t %u out of range %u..%u\n",
			*data, min, max);
		return -EINVAL;
	}
	*dest = *data;
	return 0;
}

static inline int ipc_data_int32_get(int32_t *dest, int32_t const *data,
				      int32_t const min, int32_t const max)
{
	if (*data < min || *data > max) {
		ipct_err("error: int32_t %d out of range %d..%d\n",
			*data, min, max);
		return -EINVAL;
	}
	*dest = *data;
	return 0;
}

static inline int ipc_data_uint16_get(uint16_t *dest, uint16_t const *data,
				      uint16_t const min, uint16_t const max)
{
	if (*data < min || *data > max) {
		ipct_err("error: uint16_t %u out of range %u..%u\n",
			*data, min, max);
		return -EINVAL;
	}
	*dest = *data;
	return 0;
}

static inline int ipc_data_int16_get(int16_t *dest, int16_t const *data,
				      int16_t const min, int16_t const max)
{
	if (*data < min || *data > max) {
		ipct_err("error: int16_t %d out of range %d..%d\n",
			*data, min, max);
		return -EINVAL;
	}
	*dest = *data;
	return 0;
}

static inline int ipc_data_uint8_get(uint8_t *dest, uint16_t const *data,
				      uint8_t const min, uint8_t const max)
{
	if (*data < min || *data > max) {
		ipct_err("error: uint8_t %u out of range %u..%u\n",
			*data, min, max);
		return -EINVAL;
	}
	*dest = *data;
	return 0;
}

static inline int ipc_data_int8_get(int8_t *dest, uint16_t const  *data,
				      int8_t const min, int8_t const max)
{
	if (*data < min || *data > max) {
		ipct_err("error: int8_t %d out of range %d..%d\n",
			(int8_t)(*data), min, max);
		return -EINVAL;
	}
	*dest = *data;
	return 0;
}

static inline int ipc_data_double_get(double *dest, double const *data,
				      double const min, double const max)
{
	if (*data < min || *data > max) {
		ipct_err("error: double %f out of range %f..%f\n",
			*data, min, max);
		return -EINVAL;
	}
	*dest = *data;
	return 0;
}

static inline int ipc_data_float_get(float *dest, float const *data,
				      float const min, float const max)
{
	if (*data < min || *data > max) {
		ipct_err("error: float %f out of range %f..%f\n",
			*data, min, max);
		return -EINVAL;
	}
	*dest = *data;
	return 0;
}

static inline int ipc_data_uint64_mask_get(uint64_t *dest, uint64_t const *data,
					   uint64_t const mask)
{
	/* valid bits are in mask only */
	if (*data & ~mask) {
		ipct_err("error: mask64 value 0x%16.16lx invalid for mask 0x%16.16lx\n",
			*data, mask);
		return -EINVAL;
	}
	*dest = *data;
	return 0;
}

static inline int ipc_data_uint32_mask_get(uint32_t *dest, uint32_t const *data,
					   uint32_t const mask)
{
	/* valid bits are in mask only */
	if (*data & ~mask) {
		ipct_err("error: mask32 value 0x%8.8x invalid for mask 0x%8.8x\n",
				*data, mask);
		return -EINVAL;
	}
	*dest = *data;
	return 0;
}

static inline int ipc_data_uint16_mask_get(uint16_t *dest, uint16_t const *data,
					   uint16_t const mask)
{
	/* valid bits are in mask only */
	if (*data & ~mask) {
		ipct_err("error: mask16 value 0x%4.4x invalid for mask 0x%4.4x\n",
			*data, mask);
		return -EINVAL;
	}
	*dest = *data;
	return 0;
}

static inline int ipc_data_uint8_mask_get(uint8_t *dest, uint8_t const *data,
					   uint8_t const mask)
{
	/* valid bits are in mask only */
	if (*data & ~mask) {
		ipct_err("error: mask8 value 0x%2.2x invalid for mask 0x%2.2x\n",
			*data, mask);
		return -EINVAL;
	}
	*dest = *data;
	return 0;
}

static int elem_insert_data(const struct ipct_tuple_elem *elem,
			struct ipct_msg_context *ctx, void *tuple_data,
			uint32_t type_data_size)
{
	uint32_t elem_data_size;

	/* check that tuple data wont overflow target */
	if (type_data_size + elem->offset > ctx->src.size) {
		ipct_err("error: tuple overflows ctx->src.base\n");
		return -EINVAL;
	}

	/* is tuple data size valid ? */
	elem_data_size = elem_get_data_size(elem);
	if (!elem_data_size) {
		ipct_err("error: invalid elem data size\n");
		return -EINVAL;
	}

	/* validate that type data size matches reported size */
	if (type_data_size != elem_data_size) {
		ipct_err("error: size mismatch. tuple size %d elem size %d\n",
				type_data_size, elem_data_size);
		return -EINVAL;
	}

	/* does data exists in input buffer */
	if (!is_ptr_valid(&ctx->src, tuple_data)) {
		ipct_err("error: data outside of buffer\n");
		return -EINVAL;
	}
	if (!is_ptr_valid(&ctx->src, tuple_data + elem_data_size)) {
		ipct_err("error: data outside of buffer\n");
			ipct_log("!! %p %p\n",
			tuple_data + elem_data_size, ctx->src.base + ctx->src.size);
		return -EINVAL;
	}

	elem_printf(elem, tuple_data);

	switch (elem->type) {
	case ipct_type_int8_value:
		return ipc_data_int8_get(ctx->dest.base + elem->offset,
				(uint16_t const *)tuple_data,
				elem->value1, elem->value2);
		break;
	case ipct_type_uint8_value:
		return ipc_data_uint8_get(ctx->dest.base + elem->offset,
				(uint16_t const *)tuple_data,
				elem->value1, elem->value2);
		break;
	case ipct_type_int16_value:
		return ipc_data_int16_get(ctx->dest.base + elem->offset,
				(int16_t const *)tuple_data,
				elem->value1, elem->value2);
		break;
	case ipct_type_uint16_value:
	case ipct_type_boolean:
		return ipc_data_uint16_get(ctx->dest.base + elem->offset,
				(uint16_t const *)tuple_data,
				elem->value1, elem->value2);
		break;
	case ipct_type_int32_value:
		return ipc_data_int32_get(ctx->dest.base + elem->offset,
				(int32_t const *)tuple_data,
				elem->value1, elem->value2);
		break;
	case ipct_type_uint32_value:
	case ipct_type_enum:
		return ipc_data_uint32_get(ctx->dest.base + elem->offset,
				(uint32_t const *)tuple_data,
				elem->value1, elem->value2);
		break;
	case ipct_type_int64_value:
		return ipc_data_int64_get(ctx->dest.base + elem->offset,
				(int64_t const *)tuple_data,
				elem->value1, elem->value2);
		break;
	case ipct_type_uint64_value:
		return ipc_data_uint64_get(ctx->dest.base + elem->offset,
				(uint64_t const *)tuple_data,
				elem->value1, elem->value2);
		break;
	case ipct_type_float_value:
		return ipc_data_float_get(ctx->dest.base + elem->offset,
				(float const *)tuple_data,
				elem->value1, elem->value2);
		break;
	case ipct_type_double_value:
		return ipc_data_double_get(ctx->dest.base + elem->offset,
				(double const *)tuple_data,
				elem->value1, elem->value2);
		break;
	case ipct_type_uint8_mask:
		return ipc_data_uint8_mask_get(ctx->dest.base + elem->offset,
				(uint8_t const *)tuple_data,
				elem->value1);
		break;
	case ipct_type_uint16_mask:
		return ipc_data_uint16_mask_get(ctx->dest.base + elem->offset,
				(uint16_t const *)tuple_data,
				elem->value1);
		break;
	case ipct_type_uint32_mask:
		return ipc_data_uint32_mask_get(ctx->dest.base + elem->offset,
				(uint32_t const *)tuple_data,
				elem->value1);
		break;
	case ipct_type_uint64_mask:
		return ipc_data_uint64_mask_get(ctx->dest.base + elem->offset,
				(uint64_t const *)tuple_data,
				elem->value1);
		break;
	case ipct_type_string:
	case ipct_type_uuid:
	case ipct_type_data:
		return ipc_data_get(ctx->dest.base + elem->offset,
				   (uint32_t const *)tuple_data,
					elem->value1, elem_data_size);
		break;
	default:
		/* already checked above - should never get here */
		return -EINVAL;
	}
}


/* unpack a tuple into a C ctx->src.base member */
static int tuple_unpack(const struct ipct_tuple *tuple,
			const struct ipct_action_def *action,
			struct ipct_msg_context *ctx)
{
	const struct ipct_tuple_elem *elem;
	uint32_t type_data_size;
	void *tuple_data;
	int i, ret;
	uint32_t elem_count;

	ipct_log("  unpack: tuple type %d id %d\n",
		tuple->type, tuple->id);

	/* check is tuple array */
	elem_count = tuple_data_count(tuple);
	if (elem_count == 0) {
		ipct_log("unpack: tuple id %d has no data\n", tuple->id);
		return -EINVAL;
	}

	/* is tuple data size valid ? */
	type_data_size = tuple_data_size(tuple);
	if (!type_data_size) {
		ipct_err("error: invalid tuple size for id %d\n",
			tuple->id);
		return -EINVAL;
	}

	/* for each tuple data element */
	for (i = 0; i < elem_count; i++) {

		/* is tuple found ? */
		elem = get_tuple_elem(action->desc, tuple, i, 0);
		if (!elem) {
			ipct_log("unpack: unknown tuple id %d\n", tuple->id);
			return 0; /* no - ignore it */
		}

		log_elem_dump(__func__, elem);

		/* tuple data exists ? */
		tuple_data = tuple_get_data(tuple, i);
		if (!tuple_data) {
			ipct_err("error: cant get tuple data for id %d\n",
				tuple->id);
			return -EINVAL;
		}

		ret = elem_insert_data(elem, ctx, tuple_data, type_data_size);
		if (ret < 0) {
			ipct_err("error: can't insert tuple id %d from 0x%lx\n",
					tuple->id + i, ctx->src.offset);
			return ret;
		}
	}

	return 0;
}

static int tuple_for_each(const struct ipct_tuple *tuple,
			  const struct ipct_action_def *action_def,
			  struct ipct_msg_context *ctx, void *end_of_message,
			  uint32_t tuples_remain, int depth)
{
	const struct ipct_tuple *next;
	int remaining_bytes = end_of_message - (void *)tuple;
	int ret;

	/* check: make sure we don't recurse too deep */
	if (depth > IPCT_MAX_DEPTH) {
		ipct_err("error: ctx->src.base too deep %d\n", depth);
		return -EINVAL;
	}

	ipct_log("unpack: action 0x%x at depth %d\n",
		action_def->action_id, depth);

	/* process each tuple */
	while (tuples_remain && remaining_bytes) {

		ipct_log(" unpack: new tuple %d type %d\n",
			tuple->id, tuple->type);

		/* check: does this tuple start in message */
		if ((void *)tuple >= end_of_message){
			ipct_err("error: tuple start not in message\n");
			return -EINVAL;
		}

		/* check: does this tuple end in message */
		if ((void *)tuple +  tuple_size(tuple) > end_of_message) {
			ipct_err("error: tuple end not in message\n");
			return -EINVAL;
		}

		if (tuple->type == IPCT_TUPLE_TYPE_TUPLE_ARRAY) {

			/* unpack this tuple */
			ret = tuple_for_each(tuple_get_data(tuple, 0),
					action_def, ctx, end_of_message,
					tuple_data_count(tuple), depth + 1);
			if (ret < 0) {
				ipct_err("error: failed to iterate tuple\n");
				return ret;
			}
		} else {
			/* unpack this tuple */
			ret = tuple_unpack(tuple, action_def, ctx);
			if (ret < 0) {
				ipct_err("error: failed to unpack tuple\n");
				return ret;
			}
		}

		/* get next tuple */
		next = ipc_next_tuple(tuple);
		if (next <= tuple) {
			ipct_err("error: illegal next tuple\n");
			return -EINVAL;
		}

		remaining_bytes -= tuple_size(tuple);
		tuples_remain--;
		tuple = next;
	}

	return 0;
}

/**
 * Convert message from tuples to internal C ctx->src.base.
 *
 * This takes **completely untrusted** data as input and copies it field by
 * filed into a local C ctx->src.base.
 */
int ipct_unpack(struct ipct_msg_context *ctx)
{
	const struct ipct_action_def *action_def;
	const struct ipct_tuple *tuple;
	struct ipct_hdr *hdr = ctx->src.base;
	uint32_t num_tuples;
	void *end_of_message;
	uint32_t size;

	int ret = 0;

	/* validate ID - is it supported ?*/
	action_def = get_action_def(ctx->id);
	if (!action_def) {
		ipct_err("ipct: error can't find action 0x%x\n", ctx->id);
		return -EINVAL;
	}

	/* stream config expects tuples */
	tuple = IPCT_HDR_GET_ELEM_PTR(hdr);
	if (!tuple) {
		ipct_err("ipct: error can't find tuple for action 0x%x\n", ctx->id);
		return -EINVAL;
	}

	/* we have tuples, but how many ? */
	num_tuples = ipct_get_tuples(hdr);
	if (num_tuples == 0) {
		ipct_err("ipct: error action 0x%x has no tuples\n", ctx->id);
		return -EINVAL;
	}

	/* validate size - size is mandatory */
	size = ipct_get_size(hdr);
	if (size == 0) {
		ipct_err("ipct: error can't find size for action 0x%x\n", ctx->id);
		return -EINVAL;
	}

	/* validate against minimum size */
	if (size < tuple_size(tuple)) {
		ipct_err("ipct: error action 0x%x too small\n", ctx->id);
		return -EINVAL;
	}

	/* calculate end of message */
	end_of_message = (void *)hdr + IPCT_HDR_GET_HDR_SIZE(hdr) + size;
	tuple = IPCT_HDR_GET_TUPLE(hdr);

	/* unpack tuples */
	ret = tuple_for_each(tuple, action_def, ctx, end_of_message, num_tuples, 0);
	if (ret < 0)
		ipct_err("ipct: failed to unpack\n");

	/* finished */
	return ret;
}
