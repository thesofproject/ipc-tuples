/* SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright(c) 2020 Intel Corporation. All rights reserved.
 *
 * Author: Liam Girdwood <liam.r.girdwood@linux.intel.com>
 */

#ifndef _IPCT_CLIENT_H_
#define _IPCT_CLIENT_H_

#include <stdint.h>
#include <stddef.h>

#include <abi/header.h>
#include <abi/message.h>

#define IPCT_ACTION_ID(klass, subklass, action) \
	((klass << 0) | (subklass << 8) | (action << 16))

#define IPCT_FLAGS_NONE		(0 << 24)
#define IPCT_FLAGS_PRIORTY	(1 << 25)
#define IPCT_FLAGS_DATAGRAM	(1 << 26)
#define IPCT_FLAGS_BROADCAST	(1 << 27)
#define IPCT_FLAGS_REPLY_NACK	(1 << 28)
#define IPCT_FLAGS_REPLY_ACK	(1 << 29)

int ipct_msg_pack(uint32_t id, void *src, size_t src_size,
		  void *dest, size_t dest_size,
		  uint32_t flags, uint32_t dest_addr);

int ipct_msg_unpack(void *src, size_t src_size,
		  void *dest, size_t dest_size,
		  uint32_t *id, uint32_t *dest_addr);


#endif /* _IPCT_CLIENT_H_ */
