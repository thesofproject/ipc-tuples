/* SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright(c) 2020 Intel Corporation. All rights reserved.
 *
 */

#ifndef SRC_INCLUDE_IPCT_BUILDER_H_
#define SRC_INCLUDE_IPCT_BUILDER_H_

/*
 * TODO: Split PRIVATE IPCT HEADER - Used internally and NOT by client code.
 */

#include <stdint.h>
#include <stddef.h>

#include <private/header.h>
#include <private/message.h>

struct ipct_action_struct_desc;

/*
 * IPCT uses the extended manifest to tell the kernel about the supported
 * firmware features and IPC ABI. The extended manifest is built using the
 * following convenience structures and macros.
 */

/* size of array in bytes - avoids naming collisions */
#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x)	(sizeof(x) / sizeof(x[0]))
#endif

/*
 * IPCT tuple data types.
 */
enum ipct_tuple_data_type {
	ipct_type_int8_value		= 0,
	ipct_type_uint8_value		= 1,
	ipct_type_int16_value		= 2,
	ipct_type_uint16_value		= 3,
	ipct_type_int32_value		= 4,
	ipct_type_uint32_value		= 5,
	ipct_type_int64_value		= 6,
	ipct_type_uint64_value		= 7,
	ipct_type_float_value		= 8,
	ipct_type_double_value		= 9,
	ipct_type_uint8_mask		= 10,
	ipct_type_uint16_mask		= 11,
	ipct_type_uint32_mask		= 12,
	ipct_type_uint64_mask		= 13,
	ipct_type_enum			= 14,
	ipct_type_boolean		= 15,
	ipct_type_string		= 16,
	ipct_type_uuid			= 17,
	ipct_type_data			= 18,
};

/*
 * IPCT Tuple Element.
 * 
 * Maps a C structure member to a tuple and includes information about type
 * and how to validate the value.
 */

struct ipct_tuple_elem {
	uint16_t id;		/* mandatory - tuple ID */
	uint16_t type;		/* mandatory - ipct_tuple_type */
	uint32_t offset;	/* mandatory - offset (bytes) in C struct */
	unsigned long value1;	/* optional - validation value 1 */
	unsigned long value2;	/* optional - validation value 2 */
};

/* Convenience macro for creating tuple elements - ipct_tuple_elem */
#define IPCT_TUPLE_ELEM(tid, ttype, toffset, tval1, tval2)		\
{									\
	.id		= tid,						\
	.type		= ttype,					\
	.offset		= toffset,					\
	.value1		= tval1,					\
	.value2		= tval2,					\
}

/* Convenience to build an array of tuple elems */
#define IPCT_DECLARE_TUPLE_ELEMS(tname, ...)				\
	const struct ipct_tuple_elem tname ## _tuples[] = {		\
		__VA_ARGS__						\
}

/*
 * IPCT Tuple Set.
 *
 * A set of tuples. Can have similar attributes.
 */
struct ipct_tuple_set {
	int count;
	const struct ipct_tuple_elem *elem;
};

/* Convenience to build a populated or empty set of tuples - ipct_tuple_set */
#define IPCT_TUPLES(tname)						\
	{ARRAY_SIZE(tname ## _tuples), tname ## _tuples}

#define IPCT_NOTUPLES							\
	{0, NULL}


/*
 * IPCT Action Descriptor Set.
 *
 * A set of action descriptors. Can have similar attributes.
 */
struct ipct_action_set {
	int count;
	const struct ipct_action_struct_desc *action_desc;
};

/* Convenience to build a populated or empty set of actions - ipct_action_set */
#define IPCT_SUBACTION(sname)						\
	{ARRAY_SIZE(sname ## _subaction_desc), sname ## _subaction_desc}

#define IPCT_NOSUBACTION						\
	{0, NULL}

/*
 * IPCT Action Descriptor
 *
 * Maps C structures to IPC action. The source C structure(s) can be nested
 * or within arrays.
 */
struct ipct_action_struct_desc {
	int array_elems;	/* number of elems in this array or 0 */

	unsigned int size;	/* size of C struct */
	unsigned long offset;	/* in parent C struct */

	struct ipct_action_set subaction;

	struct ipct_tuple_set mandatory;
	struct ipct_tuple_set optional;
};

/* Convenience to build an array of subaction (child C struct) action elems */
#define IPCT_DECLARE_SUBACTION_ELEM(aname, sparent, sman, sopt, sarray, schild)	\
{									\
	.size		= sizeof(((struct sparent *)0)->aname),		\
	.offset		= offsetof(struct sparent, aname),		\
	.subaction	= schild,					\
	.array_elems	= sarray,					\
	.mandatory	= sman,						\
	.optional	= sopt,						\
}

/* Convenience to declare a subaction */
#define IPCT_DECLARE_SUBACTION_DESC(sname, ...)					\
	const struct ipct_action_struct_desc sname ## _subaction_desc[] = {	\
			__VA_ARGS__						\
}

/* Convenience to build an action array */
#define IPCT_DECLARE_ACTION_DESC(astruct, sman, sopt, sarray, schild)		\
	const struct ipct_action_struct_desc astruct ## _action_desc = {	\
		.size		= sizeof(struct astruct),			\
		.subaction	= schild,					\
		.array_elems	= sarray,					\
		.mandatory	= sman,						\
		.optional	= sopt,						\
}

/*
 * IPCT Action Definition - Used by client code.
 *
 * Describes an IPC action with optional and mandatory tuple IDs required by
 * the action to be properly constructed.
 *
 * Also contains mandatory and optional client call backs when action is
 * being parsed and processed.
 */
struct ipct_action_def {
	uint32_t action_id;		/**< action ID - maps to IPC message action */

	const struct ipct_action_struct_desc *desc;
};

#define IPCT_ACTION(aid, aname)							\
		{.action_id = aid, .desc = &aname ## _action_desc}

#define IPCT_DECLARE_ACTIONS(saction, ...)				\
	const struct ipct_action_def saction ## _actions[] = {	\
		__VA_ARGS__						\
}

/*
 * IPCT Feature Sub Class - (spelled with a K for C++ users)
 *
 * Describes the capabilities of a use case within a feature driver.
 */
struct ipct_subklass_def {
	uint32_t subclass_id;		/**< subclass ID - use to map feature use case */
	uint32_t num_actions;		/**< number of actions in this subclass */

	/* action data follows here */
	const struct ipct_action_def *actions;	/**< actions - size is num_actions */
};

#define IPCT_DECLARE_SUBCLASS(sname, sid, sactions)			\
	const struct ipct_subklass_def sname ## _subclass = {	\
		.subclass_id	= sid,					\
		.num_actions	= ARRAY_SIZE(sactions),			\
		.actions	= sactions,				\
}

/*
 * IPCT Feature Class - (spelled with a K for C++ users)
 *
 * Describes the capabilities of a feature driver.
 */
struct ipct_klass_def {
	uint32_t klass_id;		/**< class ID - used to match feature driver */
	uint32_t num_subklasses;	/**< number of subclasses in this class */

	/* sub class data follows here */
	const struct ipct_subklass_def *subklass;	/*<< subklasses - size is num_subclasses */
};

/*
 * IPCT Array of Feature Classes - (spelled with a K for C++ users)
 *
 * Describes all the feature driver classses.
 */
struct ipct_klass_list {
	uint32_t num_klasses;	/**< number of classes in this class */
	const struct ipct_klass_def *klasses;
};

extern struct ipct_klass_list builder_klasses;

static inline enum ipct_tuple_elem_type ipct_get_type(enum ipct_tuple_data_type type)
{
	switch (type) {
	case ipct_type_int8_value:
	case ipct_type_uint8_value:
	case ipct_type_uint8_mask:
	case ipct_type_int16_value:
	case ipct_type_uint16_value:
	case ipct_type_uint16_mask:
	case ipct_type_boolean:
		return IPCT_TUPLE_TYPE_HD;
	case ipct_type_int32_value:
	case ipct_type_uint32_value:
	case ipct_type_uint32_mask:
	case ipct_type_enum:
	case ipct_type_int64_value:
	case ipct_type_uint64_value:
	case ipct_type_uint64_mask:
	case ipct_type_float_value:
	case ipct_type_double_value:
	case ipct_type_uuid:
	case ipct_type_string:
	case ipct_type_data:
	default:
		return IPCT_TUPLE_TYPE_STD;
	}
}

static inline uint32_t elem_get_data_size(const struct ipct_tuple_elem *elem)
{
	switch (elem->type) {
	case ipct_type_int8_value:
	case ipct_type_uint8_value:
	case ipct_type_uint8_mask:
	case ipct_type_int16_value:
	case ipct_type_uint16_value:
	case ipct_type_uint16_mask:
	case ipct_type_boolean:
		return sizeof(uint16_t);
	case ipct_type_int32_value:
	case ipct_type_uint32_value:
	case ipct_type_uint32_mask:
	case ipct_type_enum:
		return sizeof(uint32_t);
	case ipct_type_int64_value:
	case ipct_type_uint64_value:
	case ipct_type_uint64_mask:
		return sizeof(uint64_t);
	case ipct_type_float_value:
		return sizeof(float);
	case ipct_type_double_value:
		return sizeof(double);
	case ipct_type_uuid:
		return sizeof(uint32_t) * 4;
	case ipct_type_string:
	case ipct_type_data:
		/* get max size from elem */
		return elem->value2;
	default:
		return 0;
	}
}

#endif
