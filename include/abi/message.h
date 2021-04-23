/* SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright(c) 2020 Intel Corporation. All rights reserved.
 */

/*
 * PRIVATE IPCT HEADER - Used by IPC internally and NOT by client code.
 */

#ifndef _INCLUDE_ABI_MESSAGE_H_
#define _INCLUDE_ABI_MESSAGE_H_

#include <stdint.h>

/*
 * High Level
 * ==========
 *
 * IPCT messaging works at a high level on the basic principle of a standard
 * header followed by either a tuple array of data OR a private data structure.
 *
 * Tuple Mode
 * ----------
 *
 * +------------------------+
 * | IPCT header            |
 * |   route (optional)     |
 * |   size (mandatory)     |
 * |   elems (mandatory)    |
 * +------------------------+
 * | Tuple ID | Data        |
 * +------------------------+
 * | Tuple ID | Data        |
 * +------------------------+
 * | Tuple ID | Data        |
 * +------------------------+
 * | etc ...................|
 * +------------------------+
 *
 * (example 1 - IPCT header with tuples)
 *
 * The tuple data is either fixed size or variable size and the tuples
 * are unordered in the IPC message.
 *
 * Tuple mode is enabled by setting hdr.size = 1 and hdr.elems = 1
 *
 * Tuple data can be represented by creating arrays using any combinations of
 * struct ipct_elem_std, ipct_elem_micro and ipct_elem_micro_array. This
 * provides flexibility over data expression and data density.
 *
 *
 * Private Data Mode
 * -----------------
 *
 * +------------------------+
 * | IPCT header            |
 * |   route (optional)     |
 * |   size (optional)      |
 * |   elems (optional)     |
 * +------------------------+
 * | Private data block     |
 * +------------------------+
 *
 * (example 2 - IPCT header with private data)
 *
 * The private data can be anything - The primary use case is C data structures
 * from previous IPC ABIs. Private data only mode does not use the tuples below
 * but uses existing or legacy IPC ABI structures.
 *
 * Private data block only mode is enabled by setting hdr.block = 1.
 *
 *
 * Tuples
 * ======
 *
 * Tuples come in two types where type is determined by MSB of tuple ID. This
 * is to provide flexibility for message density and data size. i.e. small
 * tuple dense messages are supported alongside large messages with variable
 * tuple density.
 *
 * 1) Standard tuple element - Minimum 2 words (1 data word) - max 256kB
 *
 * 2) Micro tuple element - Fixed size 1 word (1 data short).
 *
 * Where the tuple IDs are in a continuous range then the protocol can compress
 * tuples and omit IDs for each tuple are ID[0] (the base tuple ID) meaning the
 * tuple array is data only with each subsequent word/short being the next
 * element in the array.
 *
 * Tuple IDs
 * ---------
 *
 * The tuple ID is a 15bit number unique only to the class, subclass, AND action
 * meaning each action can have up to 2^14 standard tuple data elements and 2^14
 * micro tuple elements or 2^15 data element IDs in total.
 *
 * This tuple ID range gives enough head room for ID deprication and new ID
 * additions without having to add and code new actions.
 */

/*
 * IPCT Tuple Type
 *
 * Type defines the tuple data size and container as described below.
 */
enum ipct_tuple_elem_type {
	IPCT_TUPLE_TYPE_STD		= 0, /* struct ipct_elem_std */
	IPCT_TUPLE_TYPE_HD		= 1, /* struct ipct_elem_micro */
	IPCT_TUPLE_TYPE_STD_ARRAY	= 2, /* struct ipct_elem_std */
	IPCT_TUPLE_TYPE_HD_ARRAY	= 3, /* struct ipct_elem_micro_array */
	IPCT_TUPLE_TYPE_VAR_ARRAY	= 4, /* struct ipct_elem_var_array */
	IPCT_TUPLE_TYPE_TUPLE_ARRAY	= 5, /* struct ipct_elem_var_array */
	IPCT_TUPLE_TYPE_RESERVED2	= 6,
	IPCT_TUPLE_TYPE_RESERVED3	= 7, /* last type due to 3 bit size */
};

/* maximum ID a tuple can have */
#define IPCT_TUPLE_MAX_ID	0x1FFF

/*
 * IPCT.0 Tuple ID.
 *
 * The tuple ID has a 13 bit ID value and a 3 bit type flag indicating the
 * tuple structure used as a container.
 */

struct ipct_tuple {
	uint16_t type : 3;	/* tuple type - enum ipct_tuple_elem_type */
	uint16_t id : 13;	/* tuple ID - specific to class/subclass/action */
} __attribute__((packed, aligned(2)));


/*
 * IPCT.0 - generic "standard" tuple data element.
 *
 * Standard tuple type that can be used for either a single tuple
 * (tuple.type = IPCT_TUPLE_TYPE_STD) or for an array of standard tuples
 * (tuple.type = IPCT_TUPLE_TYPE_STD_ARRAY).
 *
 * Single tuple mode can represent from 4 bytes to 256kB of data.
 * Array mode can represent a continuous array of 2^16 tuples starting at
 * tuple.id of element size 4 bytes.
 */
struct ipct_elem_std {
	struct ipct_tuple tuple;	/* tuple ID and type = IPCT_TUPLE_TYPE_STD | STD_ARRAY */
	union {
		uint16_t size;		/* data size in words  - IPCT_TUPLE_TYPE_STD */
		uint16_t count;		/* number of elems - IPCT_TUPLE_TYPE_STD_ARRAY */
	};
	uint32_t data[];		/* tuple data */
} __attribute__((packed, aligned(4)));

/*
 * IPCT.0 - Micro tuple element.
 *
 * Micro tuple type that can be used for 2 bytes of data only.
 * (tuple.type = IPCT_TUPLE_TYPE_HD)
 */
struct ipct_elem_micro {
	struct ipct_tuple tuple;	/* tuple ID and type */
	uint16_t data;			/* tuple data */
} __attribute__((packed, aligned(4)));

/*
 * IPCT.0 - Micro tuple element array.
 *
 * Micro tuple array type that can be used for array of 2 byte data.
 * (tuple.type = IPCT_TUPLE_TYPE_HD_ARRAY). Can represent a continuous array
 * of 2^16 tuples starting at tuple.id of element size 2 bytes.
 */
struct ipct_elem_micro_array {
	struct ipct_tuple tuple;	/* tuple ID (of array[0]) and type */
	uint16_t count;			/* tuple array count */
	uint16_t data[];		/* tuple data */
} __attribute__((packed, aligned(4)));

/*
 * IPCT.0 - Variable tuple element array.
 *
 * Variable tuple array type that can be used for array of variable size byte data.
 * (tuple.type = IPCT_TUPLE_TYPE_VAR_ARRAY). Can represent a continuous array
 * of 2^16 tuples starting at tuple.id of element size 2 bytes.
 */
struct ipct_elem_var_array {
	struct ipct_tuple tuple;	/* tuple ID (of array[0]) and type */
	uint16_t count;			/* tuple array count */
	uint16_t elem_bytes;		/* array element size in bytes */
	uint8_t data[];			/* tuple data */
} __attribute__((packed, aligned(4)));

#endif /* _INCLUDE_ABI_MESSAGE_H_ */

