/* SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright(c) 2020 Intel Corporation. All rights reserved.
 *
 * Author: Liam Girdwood <liam.r.girdwood@linux.intel.com>
 */

#include "../include/ipct/client.h"

#include <stdint.h>
#include <errno.h>
#include <stdio.h>
#include <assert.h>

#include <ipct/builder.h>
#include "priv.h"

/* IPCT klasses */
static const struct ipct_klass_list *features = &builder_klasses;

const struct ipct_action_def *get_action(const struct ipct_subklass_def *subklass,
						uint32_t id)
{
	const struct ipct_action_def *action;
	uint32_t id_action = IPCT_ID_GET_ACTION(id);
	int i;

	/* iterate all actions for matching ID */
	for (i = 0; i < subklass->num_actions; i++) {
		action = &subklass->actions[i];
		if (id_action == action->action_id)
			return action;
	}

	/* not found */
	ipct_err("error: can't find action 0x%x from ID 0x%x\n",
		id_action, id);
	return NULL;
}

const struct ipct_subklass_def *get_subklass(const struct ipct_klass_def *klass,
						    uint32_t id)
{
	const struct ipct_subklass_def *subklass;
	uint32_t id_subklass = IPCT_ID_GET_SUBKLASS(id);
	int i;

	/* iterate all subklasses for matching ID */
	for (i = 0; i < klass->num_subklasses; i++) {
		subklass = &klass->subklass[i];
		if (id_subklass == subklass->subclass_id)
			return subklass;
	}

	/* not found */
	ipct_err("error: can't find subklass 0x%x from ID 0x%x\n",
		id_subklass, id);
	return NULL;
}

const struct ipct_klass_def *get_klass(uint32_t id)
{
	const struct ipct_klass_def *klass;
	uint32_t id_klass = IPCT_ID_GET_KLASS(id);
	int i;

	/* iterate all klasses for matching ID */
	for (i = 0; i <features->num_klasses; i++) {
		klass = &features->klasses[i];
		if (id_klass == klass->klass_id)
			return klass;
	}

	/* not found */
	ipct_err("error: can't find klass 0x%x from ID 0x%x\n",
		id_klass, id);
	return NULL;
}

const struct ipct_action_def *get_action_def(uint32_t id)
{
	const struct ipct_subklass_def *subklass;
	const struct ipct_klass_def *klass;

	klass = get_klass(id);
	if (!klass)
		return NULL;

	subklass = get_subklass(klass, id);
	if (!subklass)
		return NULL;

	return get_action(subklass, id);
}

const struct ipct_tuple_elem *get_tuple_elem(const struct ipct_action_struct_desc *desc,
					     const struct ipct_tuple *tuple, int index,
					     int depth)
{
	const struct ipct_action_struct_desc *subaction;
	const struct ipct_tuple_elem *elem;
	int i;

	/* check mandatory first */
	for (i = 0; i < desc->mandatory.count; i++) {
		elem = &desc->mandatory.elem[i];
		if (elem->id == tuple->id + index)
			return elem;
	}

	/* check optional next */
	for (i = 0; i < desc->optional.count; i++) {
		elem = &desc->optional.elem[i];
		if (elem->id == tuple->id + index)
			return elem;
	}

	/* not found ? - check sub structures */
	for (i = 0; i < desc->subaction.count; i++) {
		subaction = &desc->subaction.action_desc[i];
		elem = get_tuple_elem(subaction, tuple, index, depth + 1);
	}

	/* cannot find elem */
	return NULL;
}


/** \brief
 *  Create an IPCT style message from source C structure and pack into
 *  destination buffer.
 */
int ipct_msg_pack(uint32_t id, void *src, size_t src_size,
		  void *dest, size_t dest_size,
		  uint32_t flags, uint32_t dest_addr)
{
	struct ipct_msg_context msg;
	int ret;

	/* setup context */
	msg.id = id;
	msg.addr = dest_addr;
	msg.flags = flags;
	msg.src.base = src;
	msg.src.offset = 0;
	msg.src.size = src_size;
	msg.dest.base = dest;
	msg.dest.offset = 0;
	msg.dest.size = dest_size;

	/* now pack the data */
	ret = ipct_pack(&msg);
	if (ret < 0)
		ipct_err("ipct: error failed to pack object 0x%x\n", id);

	return ret;
}

/** \brief
 *  received IPCT message
 */
int ipct_msg_unpack(void *src, size_t src_size,
		  void *dest, size_t dest_size,
		  uint32_t *id, uint32_t *dest_addr)
{
	struct ipct_msg_context msg;
	int ret;

	/* setup context */
	msg.id = 0;
	msg.addr = 0;
	msg.flags = 0;
	msg.src.base = src;
	msg.src.offset = 0;
	msg.src.size = src_size;
	msg.dest.base = dest;
	msg.dest.offset = 0;
	msg.dest.size = dest_size;


	ret = ipct_unpack(&msg);
	if (ret < 0)
		return ret;

	/* assign id and address if requested */
	if (id)
		*id = msg.id;
	if (dest_addr)
		*dest_addr = msg.addr;

	return 0;
}
