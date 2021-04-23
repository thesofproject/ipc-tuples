/* SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright(c) 2020 Intel Corporation. All rights reserved.
 */

/*
 * PRIVATE IPCT HEADER - Used internally and NOT by client code.
 */

#ifndef _IPCT_PRIVATE_MESSAGE_H_
#define _IPCT_PRIVATE_MESSAGE_H_

#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <abi/message.h>

#define _IPC_TO_TUPLE(x) \
	 __builtin_assume_aligned (x, 2)
#define IPC_GET_STD_TUPLE(x) \
	(struct ipct_elem_std *)_IPC_TO_TUPLE(x)
#define IPC_GET_STD_TUPLE_ARRAY(x) \
	(struct ipct_elem_var_array *)_IPC_TO_TUPLE(x)
#define IPC_GET_MICRO_TUPLE(x) \
	(struct ipct_elem_micro *)_IPC_TO_TUPLE(x)
#define IPC_GET_MICRO_TUPLE_ARRAY(x) \
	(struct ipct_elem_micro_array *)_IPC_TO_TUPLE(x)
#define IPC_GET_VAR_TUPLE_ARRAY(x) \
	(struct ipct_elem_var_array *)_IPC_TO_TUPLE(x)

/*
 * Size of tuple structure and data in bytes.
 */
static inline uint32_t tuple_size(const struct ipct_tuple *tuple)
{
	const struct ipct_elem_std *std;
	const struct ipct_elem_micro_array *micro;
	const struct ipct_elem_var_array *var;

	switch (tuple->type) {
	case IPCT_TUPLE_TYPE_STD:
		std = IPC_GET_STD_TUPLE(tuple);
		return sizeof(struct ipct_elem_std) +  std->size * sizeof(uint32_t);
	case IPCT_TUPLE_TYPE_HD:
		return sizeof(struct ipct_elem_micro);
	case IPCT_TUPLE_TYPE_STD_ARRAY:
		std = IPC_GET_STD_TUPLE(tuple);
		return sizeof(struct ipct_elem_std) +  std->count * sizeof(uint32_t);
	case IPCT_TUPLE_TYPE_HD_ARRAY:
		micro = IPC_GET_MICRO_TUPLE_ARRAY(tuple);
		return sizeof(struct ipct_elem_micro_array) +  micro->count * sizeof(uint16_t);
	case IPCT_TUPLE_TYPE_VAR_ARRAY:
	case IPCT_TUPLE_TYPE_TUPLE_ARRAY:
		var = IPC_GET_VAR_TUPLE_ARRAY(tuple);
		return sizeof(struct ipct_elem_var_array) +
			var->count + var->elem_bytes;
	default:
		return 0;
	}
}

/*
 * Size of tuple data only in bytes.
 */
static inline uint32_t tuple_data_size(const struct ipct_tuple *tuple)
{
	const struct ipct_elem_var_array *var;

	switch (tuple->type) {
	case IPCT_TUPLE_TYPE_STD:
		return sizeof(uint32_t);
	case IPCT_TUPLE_TYPE_HD:
		return sizeof(uint16_t);
	case IPCT_TUPLE_TYPE_STD_ARRAY:
		return sizeof(uint32_t);
	case IPCT_TUPLE_TYPE_HD_ARRAY:
		return sizeof(uint16_t);
	case IPCT_TUPLE_TYPE_VAR_ARRAY:
	case IPCT_TUPLE_TYPE_TUPLE_ARRAY:
		var = IPC_GET_VAR_TUPLE_ARRAY(tuple);
		return var->elem_bytes;
	default:
		return 0;
	}
}

/*
 * Size of tuple data only in bytes.
 */
static inline uint32_t tuple_payload_size(const struct ipct_tuple *tuple)
{
	const struct ipct_elem_std *std;
	const struct ipct_elem_micro_array *micro;
	const struct ipct_elem_var_array *var;

	switch (tuple->type) {
	case IPCT_TUPLE_TYPE_STD:
		std = IPC_GET_STD_TUPLE(tuple);
		return std->size * sizeof(uint32_t);
	case IPCT_TUPLE_TYPE_HD:
		return sizeof(uint16_t);
	case IPCT_TUPLE_TYPE_STD_ARRAY:
		std = IPC_GET_STD_TUPLE(tuple);
		return std->count * sizeof(uint32_t);
	case IPCT_TUPLE_TYPE_HD_ARRAY:
		micro = IPC_GET_MICRO_TUPLE_ARRAY(tuple);
		return micro->count * sizeof(uint16_t);
	case IPCT_TUPLE_TYPE_VAR_ARRAY:
	case IPCT_TUPLE_TYPE_TUPLE_ARRAY:
		var = IPC_GET_VAR_TUPLE_ARRAY(tuple);
		return var->count + var->elem_bytes;
	default:
		return 0;
	}
}

/*
 * Size of tuple data array in elems.
 */
static inline uint32_t tuple_data_count(const struct ipct_tuple *tuple)
{
	const struct ipct_elem_std *std;
	const struct ipct_elem_micro_array *micro;
	const struct ipct_elem_var_array *var;

	switch (tuple->type) {
	case IPCT_TUPLE_TYPE_STD:
	case IPCT_TUPLE_TYPE_HD:
		return 1;
	case IPCT_TUPLE_TYPE_STD_ARRAY:
		std = IPC_GET_STD_TUPLE(tuple);
		return std->count;
	case IPCT_TUPLE_TYPE_HD_ARRAY:
		micro = IPC_GET_MICRO_TUPLE_ARRAY(tuple);
		return micro->count;
	case IPCT_TUPLE_TYPE_VAR_ARRAY:
	case IPCT_TUPLE_TYPE_TUPLE_ARRAY:
		var = IPC_GET_VAR_TUPLE_ARRAY(tuple);
		return var->count;
	default:
		return 0;
	}
}

/*
 * Pointer to tuple data.
 */
static inline void* tuple_get_data(const struct ipct_tuple *tuple, int index)
{
	struct ipct_elem_std *std;
	struct ipct_elem_micro *micro;
	struct ipct_elem_micro_array *micro_array;
	struct ipct_elem_var_array *var;

	switch (tuple->type) {
	case IPCT_TUPLE_TYPE_STD:
	case IPCT_TUPLE_TYPE_STD_ARRAY:
		std = IPC_GET_STD_TUPLE(tuple);
		return &std->data[index];
	case IPCT_TUPLE_TYPE_HD:
		micro = IPC_GET_MICRO_TUPLE(tuple);
		return &micro->data;
	case IPCT_TUPLE_TYPE_HD_ARRAY:
		micro_array = IPC_GET_MICRO_TUPLE_ARRAY(tuple);
		return &micro_array->data[index];
	case IPCT_TUPLE_TYPE_VAR_ARRAY:
	case IPCT_TUPLE_TYPE_TUPLE_ARRAY:
		var = IPC_GET_VAR_TUPLE_ARRAY(tuple);
		return var->data + index * var->elem_bytes;
	default:
		return NULL;
	}
}

/*
 * Init
 */
static inline int tuple_init(struct ipct_tuple *tuple,
			      enum ipct_tuple_elem_type type,
			      uint16_t id, uint16_t size)
{
	struct ipct_elem_std *std;
	struct ipct_elem_micro *micro;
	struct ipct_elem_micro_array *micro_array;
	struct ipct_elem_var_array *var;

	switch (tuple->type) {
	case IPCT_TUPLE_TYPE_STD:
		std = IPC_GET_STD_TUPLE(tuple);
		std->tuple.id = id;
		std->tuple.type = type;
		std->size = size >> 2; /* size is words */
		break;
	case IPCT_TUPLE_TYPE_HD:
		micro = IPC_GET_MICRO_TUPLE(tuple);
		micro->tuple.id = id;
		micro->tuple.type = type;
		break;
	case IPCT_TUPLE_TYPE_STD_ARRAY:
		std = IPC_GET_STD_TUPLE(tuple);
		std->tuple.id = id;
		std->tuple.type = type;
		std->count = size;
		break;
	case IPCT_TUPLE_TYPE_HD_ARRAY:
		micro_array = IPC_GET_MICRO_TUPLE_ARRAY(tuple);
		micro_array->tuple.id = id;
		micro_array->tuple.type = type;
		micro_array->count = size;
		break;
	case IPCT_TUPLE_TYPE_VAR_ARRAY:
	case IPCT_TUPLE_TYPE_TUPLE_ARRAY:
		var = IPC_GET_VAR_TUPLE_ARRAY(tuple);
		var->tuple.id = id;
		var->tuple.type = type;
		var->elem_bytes = size;
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

static inline int tuple_inc_elems(struct ipct_tuple *tuple)
{
	struct ipct_elem_std *std;
	struct ipct_elem_micro *micro;
	struct ipct_elem_micro_array *micro_array;
	struct ipct_elem_var_array *var;
	int size_increase = 0;

	/* convert to array type if needed */
	switch (tuple->type) {
	case IPCT_TUPLE_TYPE_STD:
		tuple->type = IPCT_TUPLE_TYPE_STD_ARRAY;
		break;
	case IPCT_TUPLE_TYPE_HD:
		micro = IPC_GET_MICRO_TUPLE(tuple);
		micro_array = IPC_GET_MICRO_TUPLE_ARRAY(tuple);
		tuple->type = IPCT_TUPLE_TYPE_HD_ARRAY;
		micro_array->data[0] = micro->data;
		micro_array->count = 1;
		size_increase = 2;
		break;
	case IPCT_TUPLE_TYPE_STD_ARRAY:
	case IPCT_TUPLE_TYPE_HD_ARRAY:
	case IPCT_TUPLE_TYPE_VAR_ARRAY:
	case IPCT_TUPLE_TYPE_TUPLE_ARRAY:
		/* already an array */
		break;
	default:
		return -EINVAL;
	}

	/* increment array count */
	switch (tuple->type) {
	case IPCT_TUPLE_TYPE_STD_ARRAY:
		std = IPC_GET_STD_TUPLE(tuple);
		std->count++;
		break;
	case IPCT_TUPLE_TYPE_HD_ARRAY:
		micro_array = IPC_GET_MICRO_TUPLE_ARRAY(tuple);
		micro_array->count++;
		break;
	case IPCT_TUPLE_TYPE_VAR_ARRAY:
	case IPCT_TUPLE_TYPE_TUPLE_ARRAY:
		var = IPC_GET_VAR_TUPLE_ARRAY(tuple);
		var->count++;
		break;
	case IPCT_TUPLE_TYPE_STD:
	case IPCT_TUPLE_TYPE_HD:
	default:
		return -EINVAL;
	}

	return size_increase;
}

/*
 * Tuple iterator
 */

static inline const struct ipct_tuple *ipc_next_tuple(const struct ipct_tuple *current)
{
	return (void*)current + tuple_size(current);
}

#endif /* _IPCT_PRIVATE_MESSAGE_H_ */
