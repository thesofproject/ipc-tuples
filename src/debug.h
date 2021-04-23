/* SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright(c) 2020 Intel Corporation. All rights reserved.
 *
 * Author: Liam Girdwood <liam.r.girdwood@linux.intel.com>
 */

#ifndef __IPCT_PRIVATE_DEBUG_H__
#define __IPCT_PRIVATE_DEBUG_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>

#include <ipct/builder.h>
#include <ipct/client.h>

#define IPCT_DEBUG

#ifdef IPCT_DEBUG

static inline void ipct_log(const char *format, ...)
{
	va_list args;

	va_start(args, format);
	vfprintf(stdout, format, args);
	va_end(args);
}

static inline void ipct_err(const char *format, ...)
{
	va_list args;

	va_start(args, format);
	vfprintf(stderr, format, args);
	va_end(args);
}

static inline void dump_raw(const char *text, void *data, size_t bytes)
{
	uint32_t *buffer = data;
	uint16_t *buffer16;
	int i, words = bytes / 4, rem = bytes % 4;

	ipct_log("%s\n", text);

	for (i = 0; i < words; i++) {

		/* dump whole 4 word lines */
		switch (i % 4) {
		case 0:
			ipct_log("0x%4.4x : %8.8x ", i << 2, buffer[i]);
			break;
		case 3:
			ipct_log("%8.8x\n", buffer[i]);
			break;
		default:
			ipct_log("%8.8x ", buffer[i]);
			break;
		}
	}

	buffer16 = (uint16_t *)&buffer[i];
	switch (rem) {
	default:
		break;
	case 2:
		ipct_log("0x%4.4x : %4.4x ", i << 2, buffer16[0]);
		break;
	}

	ipct_log("\n");
}

static inline void dump_tuple(struct ipct_tuple *tuple, uint32_t offset)
{
	int count = tuple_data_count(tuple);
	int size = tuple_data_size(tuple);
	int i;

	ipct_log("tuple header: id %d type %d items %d item size %d\n",
		tuple->id, tuple->type, count, size);

	switch (size) {
	case 4:
		for (i = 0; i < count; i++) {
			uint32_t *buffer32 = tuple_get_data(tuple, i);
			ipct_log("0x%4.4x : %8.8x\n", (i << 2) + offset, *buffer32);
		}
		break;
	case 2:
		for (i = 0; i < count; i++) {
			uint16_t *buffer16 = tuple_get_data(tuple, i);
			ipct_log("0x%4.4x : %4.4x\n", (i << 1) + offset, *buffer16);
		}
		break;
	case 1:
	case 3:
		ipct_err("tuple: illegal size %d\n", size);
		break;
	default:
		for (i = 0; i < size; i++) {

			uint32_t *buffer = tuple_get_data(tuple, i);

			/* dump whole 4 word lines */
			switch (i % 4) {
			case 0:
				ipct_log("0x%4.4x : %8.8x ", (i << 2) + offset, buffer[i]);
				break;
			case 3:
				ipct_log("%8.8x\n", buffer[i]);
				break;
			default:
				ipct_log("%8.8x ", buffer[i]);
				break;
			}
		}
		break;
	}
}

static inline void elem_printf(const struct ipct_tuple_elem *elem,
				   void *data)
{
	switch (elem->type) {
	case ipct_type_int8_value:
		ipct_log("elem: int8_t %d 0x%x\n",
			*((int8_t *)data), *((int8_t *)data));
		break;
	case ipct_type_uint8_value:
		ipct_log("elem: uint8_t %u 0x%ux\n",
			*((uint8_t *)data), *((uint8_t *)data));
		break;
	case ipct_type_uint8_mask:
		ipct_log("elem: uint8_t mask 0x%x\n",
			*((uint8_t *)data));
		break;
	case ipct_type_int16_value:
		ipct_log("elem: int16_t %d 0x%x\n",
			*((int16_t *)data), *((int16_t *)data));
		break;
	case ipct_type_uint16_value:
		ipct_log("elem: uint16_t %d 0x%x\n",
			*((uint16_t *)data), *((uint16_t *)data));
		break;
	case ipct_type_uint16_mask:
		ipct_log("elem: int16_t mask 0x%x\n",
			*((uint16_t *)data));
		break;
	case ipct_type_boolean:
		ipct_log("elem: bool %d\n",
			*((uint16_t *)data));
		break;
	case ipct_type_int32_value:
		ipct_log("elem: int32_t %d 0x%x\n",
			*((int32_t *)data), *((int32_t *)data));
		break;
	case ipct_type_uint32_value:
		ipct_log("elem: uint32_t %d 0x%x\n",
			*((uint32_t *)data), *((uint32_t *)data));
		break;
	case ipct_type_uint32_mask:
		ipct_log("elem: int32_t mask 0x%x\n",
			*((uint32_t *)data));
		break;
	case ipct_type_enum:
		ipct_log("elem: uint32_t enum %d\n",
			*((uint32_t *)data));
		break;
	case ipct_type_int64_value:
		ipct_log("elem: int64_t %ld 0x%lx\n",
			*((int64_t *)data), *((int64_t *)data));
		break;
	case ipct_type_uint64_value:
		ipct_log("elem: uint64_t %ld 0x%lx\n",
			*((uint64_t *)data), *((uint64_t *)data));
		break;
	case ipct_type_uint64_mask:
		ipct_log("elem: uint64_t mask 0x%lx\n",
			*((uint64_t *)data));
		break;
	case ipct_type_float_value:
		ipct_log("elem: float %f\n",
			*((float *)data));
		break;
	case ipct_type_double_value:
		ipct_log("elem: double %f\n",
			*((double *)data));
		break;
	case ipct_type_uuid:
		ipct_log("elem: uuid %8.8x-%4.4x-%4.4x-%2.2x%2.2x-%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x\n",
			*((uint32_t *)data),
			*((uint16_t *)(data + 4)),
			*((uint16_t *)(data + 6)),
			*((uint8_t *)(data + 8)),
			*((uint8_t *)(data + 10)),
			*((uint8_t *)(data + 12)),
			*((uint8_t *)(data + 14)),
			*((uint8_t *)(data + 16)),
			*((uint8_t *)(data + 18)),
			*((uint8_t *)(data + 20)),
			*((uint8_t *)(data + 22)));
		break;
	case ipct_type_string:
		ipct_log("elem: string %s\n",(char *)data);
		break;
	case ipct_type_data:
	default:
		return;
	}
}

static inline void log_elem_dump(const char *text,
				 const struct ipct_tuple_elem *elem)
{
	ipct_log("%s: elem id %d C offset 0x%x type %d val1 %ld val2 %ld\n",
		 text, elem->id, elem->offset, elem->type,
		 elem->value1, elem->value2);
}

static inline void log_pack_dump(const char *text,
				 size_t offset,
				 const struct ipct_tuple_elem *elem)
{
	ipct_log("%s: elem id %d C offset 0x%x to 0x%lx size 0x%x bytes\n",
		 text, elem->id, elem->offset, offset, elem_get_data_size(elem));
}
#else

static inline void log_pack_dump(const char *text,
				 struct ipct_msg_buf *buf,
				 const struct ipct_tuple_elem *elem) {}
static inline void log_elem_dump(const char *text,
				 const struct ipct_tuple_elem *elem) {}
static inline void dump_tuple(struct ipct_tuple *tuple, uint32_t offset) {}
static inline void dump_raw(const char *text, void *data, size_t bytes) {}
static inline void ipct_log(const char *format, ...) {}
static inline void ipct_err(const char *format, ...) {}
static inline void elem_printf(const struct ipct_tuple_elem *elem,
				   void *data) {}

#endif

#endif
