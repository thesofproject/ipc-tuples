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

/* pack C structure data into a new tuple */
static int ipc_pack_single(const struct ipct_action_struct_desc *action,
			   const struct ipct_tuple_elem *elem,
			   struct ipct_msg_context *ctx,
			   struct ipct_tuple **cont_tuple)
{
	struct ipct_tuple *tuple;
	uint32_t elem_data_size;
	void *tuple_data;
	enum ipct_tuple_elem_type tuple_type;
	int err;

	log_elem_dump(__func__, elem);

	/* are we a standard or high density tuple type ? */
	tuple_type = ipct_get_type(elem->type);

	/* check: is elem data size valid ? */
	elem_data_size = elem_get_data_size(elem);
	if (!elem_data_size) {
		ipct_err("error: invalid elem %d data size %d\n",
			elem->id, elem_data_size);
		return -EINVAL;
	}

	/* check: is elem offset within C structure */
	if (elem->offset + elem_data_size > ctx->src.size) {
		ipct_err("error: elem outside structure\n");
		return -EINVAL;
	}

	/* check: is tuple starting point within buffer ? */
	tuple = ctx->dest.base + ctx->dest.offset;
	if (!is_ptr_valid(&ctx->dest, tuple)) {
		ipct_err("error: tuple starts outside of buffer\n");
		return -EINVAL;
	}

	/* setup new tuple */
	err = tuple_init(tuple, tuple_type, elem->id, elem_data_size);
	if (err < 0) {
		ipct_err("error: failed to init tuple type %d\n",
			tuple_type);
		return err;
	}

	/* get new tuple data offset */
	tuple_data = tuple_get_data(tuple, 0);
	if (!tuple_data){
		ipct_err("error: failed to get tuple data\n");
		return -EINVAL;
	}

	/* this is the continuous tuple starting point */
	*cont_tuple = tuple;

	/* check: is tuple end (including it's payload) within buffer */
	if ((tuple_data + elem_data_size) >= ctx->dest.base + ctx->dest.size ||
		tuple_data < ctx->dest.base) {
		ipct_err("error: tuple outside of buffer\n");
		return -EINVAL;
	}

	/* copy the data */
	memcpy(tuple_data, ctx->src.base + elem->offset, elem_data_size);

	log_pack_dump(__func__, ctx->dest.offset, elem);

	/* return the new offset */
	return tuple_size(tuple);
}

/* pack C structure data into an existing tuple - tuple already created above */
static int ipc_pack_cont(const struct ipct_action_struct_desc *action,
			   const struct ipct_tuple_elem *elem,
			   struct ipct_msg_context *ctx,
			   struct ipct_tuple *cont_tuple)
{
	uint32_t elem_data_size;
	void *tuple_data;
	int size_increase;

	log_elem_dump(__func__, elem);

	/* check: is tuple data size valid ? */
	elem_data_size = elem_get_data_size(elem);
	if (!elem_data_size) {
		ipct_err("error: invalid elem data size\n");
		return -EINVAL;
	}

	/* check: is elem offset within C structure */
	if (elem->offset + elem_data_size > ctx->src.size) {
		ipct_err("error: elem outside structure\n");
		return -EINVAL;
	}

	/* update tuple */
	size_increase = tuple_inc_elems(cont_tuple);
	if (size_increase < 0) {
		ipct_err("error: invalid conversion to tuple array\n");
		return size_increase;
	}
	ctx->dest.offset += size_increase;

	/* check: is tuple starting point within buffer ? */
	tuple_data = ctx->dest.base + ctx->dest.offset;
	if (!is_ptr_valid(&ctx->dest, tuple_data)) {
		ipct_err("error: tuple starts outside of buffer\n");
		return -EINVAL;
	}

	/* check: is tuple end (including it's payload) within buffer */
	if ((tuple_data + elem_data_size) >= ctx->dest.base + ctx->dest.size ||
		tuple_data < ctx->dest.base) {
		ipct_err("error: tuple outside of buffer\n");
		return -EINVAL;
	}

	/* copy the data */
	memcpy(tuple_data, ctx->src.base + elem->offset, elem_data_size);

	log_pack_dump(__func__, ctx->dest.offset, elem);

	/* return the new offset */
	return tuple_data_size(cont_tuple);
}

/* get the first elem by lowest ID or NULL */
static const struct ipct_tuple_elem *get_first_elem(const struct ipct_action_struct_desc *action)
{
	const struct ipct_tuple_elem *elem, *first = NULL;
	const struct ipct_tuple_set *set;
	uint16_t id = IPCT_TUPLE_MAX_ID;
	int i;

	/* search elem by elem - mandatory first */
	set = &action->mandatory;
	for (i = 0; i < set->count; i++) {
		elem = &set->elem[i];
		if (elem->id < id) {
			first = elem;
			id = elem->id;
		}
	}

	/* search elem by elem - optional next */
	set = &action->optional;
	for (i = 0; i < set->count; i++) {
		elem = &set->elem[i];
		if (elem->id == id) {
			first = elem;
			id = elem->id;
		}
	}

	return first;
}

/*get next elem in numerical order after ID or NULL */
static const struct ipct_tuple_elem *get_next_elem(const struct ipct_action_struct_desc *action,
		const struct ipct_tuple_elem *current)
{
	const struct ipct_tuple_elem *next_elem = NULL, *elem;
	const struct ipct_tuple_set *set;
	int i;
	uint16_t next_id = IPCT_TUPLE_MAX_ID;

	/* search elem by elem - mandatory first */
	set = &action->mandatory;
	for (i = 0; i < set->count; i++) {
		elem = &set->elem[i];

		if (elem->id > current->id && elem->id < next_id) {
			next_elem = elem;
			next_id = elem->id;
		}
	}

	/* search elem by elem - optional next */
	set = &action->optional;
	for (i = 0; i < set->count; i++) {
		elem = &set->elem[i];

		if (elem->id > current->id && elem->id < next_id) {
			next_elem = elem;
			next_id = elem->id;
		}
	}

	return next_elem;
}

static const struct ipct_tuple_elem *continuous_elems(const struct ipct_action_struct_desc *action,
		const struct ipct_tuple_elem *current)
{
	const struct ipct_tuple_elem *next = current;

	/* find continuous IDs with the same tuple type */
	next = get_next_elem(action, current);
	if (!next)
		return NULL;

	/* must have sequential ID */
	if (current->id + 1 != next->id)
		return NULL;

	/* must have same type */
	if (ipct_get_type(current->type) != ipct_get_type(next->type))
		return NULL;

	return next;
}

static inline void init_header(struct ipct_msg_context *ctx)
{
	struct ipct_hdr *hdr = ctx->dest.base;

	/* set header klass, sublkass and action - clear other flags */
	*((uint32_t*)hdr) = ctx->id;

	/* set any header flags */
	hdr->elems = 1;

	ctx->dest.offset = IPCT_HDR_GET_HDR_SIZE(hdr);

	ipct_log("pack: hdr %d:%d:%d size 0x%zu\n",
		hdr->klass, hdr->subklass, hdr->action, ctx->dest.offset);
}

static inline int complete_header(struct ipct_msg_context *ctx, uint32_t tuples)
{
	struct ipct_hdr *hdr = ctx->dest.base;
	struct sof_ipct_elems *elems = IPCT_HDR_GET_ELEM_PTR(hdr);
	uint32_t offset = ctx->dest.offset;

	if (!elems) {
		ipct_err("pack: object has no elems\n");
		return -EINVAL;
	}

	elems->num_tuples = tuples;
	elems->remaining = 0;	// TODO:

	offset -= IPCT_HDR_GET_HDR_SIZE(hdr); /* subtract headers */
	offset = offset >> 2; /* convert bytes to words */

	/* validate size as elems->size is 24 bits */
	if (offset >= 1 << 24) {
		ipct_err("error: tuple too big\n");
		return -EINVAL;
	}

	elems->size = offset;

	dump_raw("pack: result", ctx->dest.base, ctx->dest.offset);

	return ctx->dest.offset;
}

static int elem_pack(const struct ipct_tuple_elem *current,
			const struct ipct_action_struct_desc *action,
			struct ipct_msg_context *ctx)
{
	const struct ipct_tuple_elem *next;
	struct ipct_tuple *cont_tuple;
	uint32_t tuples = 0;
	int cont = 0, size;

	while (current) {

		/* Continuous ? */
		if (cont) {
			size = ipc_pack_cont(action, current, ctx, cont_tuple);
		} else {
			/* all new tuples are packed on word offset */
			ctx->dest.offset += ctx->dest.offset % sizeof(uint32_t);

			size = ipc_pack_single(action, current, ctx, &cont_tuple);
		}
		if (size <= 0) {
			ipct_err("error: can't pack elem id %d type %d at 0x%x\n",
					current->id, current->type, current->offset);
			return size;
		}

		/* get next elem - check continuous first */
		next = continuous_elems(action, current);
		if (next == NULL) {
			/* new tuple */
			cont = 0;
			next = get_next_elem(action, current);
			tuples++;
		} else
			cont = 1;

		current = next;
		ctx->dest.offset += size;
	}

	return tuples;
}

/**
 * Convert message from internal C struct to tuples.
 */
int ipct_pack(struct ipct_msg_context *ctx)
{
	const struct ipct_action_def *action_def;
	const struct ipct_action_struct_desc *action;
	const struct ipct_tuple_elem *current;
	int tuples = 0;

	ipct_log("pack: id 0x%x\n",ctx-> id);

	/* validate ID - is it supported ? */
	action_def = get_action_def(ctx->id);
	if (!action_def) {
		ipct_err("ipct: error can't find action 0x%x\n", ctx->id);
		return -EINVAL;
	}
	action = action_def->desc;

	ipct_log("pack: action size %d mandatory %d optional %d\n",
		action->size, action->mandatory.count, action->optional.count);

	/* has user provided enough space to pack from ? */
	if (ctx->src.size < action->size) {
		ipct_err("ipct: error action 0x%x not enough packing space %d need %ld\n",
			ctx->id, action->size, ctx->src.size);
		return -EINVAL;
	}

	/* create header */
	init_header(ctx);

	/* process the action elem by elem - mandatory first */
	current = get_first_elem(action);
	if (!current) {
		ipct_err("error: no elems to pack in 0x%x\n", ctx->id);
		return -EINVAL;
	}

	tuples = elem_pack(current, action, ctx);
	if (tuples < 0) {
		ipct_err("error: failed to pack action 0x%x\n", ctx->id);
		return tuples;
	}

	/* finished */
	return complete_header(ctx, tuples);
}
