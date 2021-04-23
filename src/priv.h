/* SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright(c) 2020 Intel Corporation. All rights reserved.
 *
 * Author: Liam Girdwood <liam.r.girdwood@linux.intel.com>
 */

#ifndef __IPCT_PRIV_H__
#define __IPCT_PRIV_H__

#include <stdint.h>
#include <errno.h>
#include <stdio.h>
#include <assert.h>

#include <ipct/client.h>
#include <ipct/builder.h>
#include "debug.h"

/* TODO: this is generated based on IPC definitions */
#define IPCT_MAX_DEPTH		10

/*
 * IPCT has message buffers.
 *
 * 1) RX message buffer - contains tuples sent by remote which are pending to
 *                        be converted to a C local structure.
 * 2) TX message buffer - contains tuples
 */
struct ipc_msg_buf {
	void *base;
	size_t offset;
	size_t size;
};

struct ipct_context {

	int dbb_loopback;
};

struct ipct_msg_context {
	uint32_t id;
	uint32_t addr;
	uint32_t flags;
	struct ipc_msg_buf src;
	struct ipc_msg_buf dest;
};

static inline int is_ptr_valid(struct ipc_msg_buf *buf, void *ptr)
{
	/* check: ptr is within buffer */
	if (ptr < buf->base)
		return 0;
	if (ptr > buf->base + buf->size)
		return 0;

	/* ptr is within the buffer */
	return 1;
}

const struct ipct_action_def *get_action(const struct ipct_subklass_def *subklass,
						uint32_t id);

const struct ipct_subklass_def *get_subklass(const struct ipct_klass_def *klass,
						    uint32_t id);

const struct ipct_klass_def *get_klass(uint32_t id);

const struct ipct_action_def *get_action_def(uint32_t id);

const struct ipct_tuple_elem *get_tuple_elem(const struct ipct_action_struct_desc *desc,
					     const struct ipct_tuple *tuple, int index, int depth);

int ipct_pack(struct ipct_msg_context *ctx);
int ipct_unpack(struct ipct_msg_context *ctx);

#endif
