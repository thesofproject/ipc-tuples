/* SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright(c) 2020 Intel Corporation. All rights reserved.
 *
 * Author: Liam Girdwood <liam.r.girdwood@linux.intel.com>
 */

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

/* common framework */
#include "shared/classes.h"
#include "shared/stream.h"

#include "fw/stream.h"
#include "test.h"
#include <ipct/builder.h>
#include <ipct/client.h>

#define IPC_MAX_MSG_SIZE	1024

/* FW receives new IPC from SW */
void test_send_ipc_to_fw(void *data, int size)
{
	char unpacked_data[IPC_MAX_MSG_SIZE];	/* local buffer */
	uint32_t id, addr;
	int ret;

	ret = ipct_msg_unpack(data, size, unpacked_data,
			     IPC_MAX_MSG_SIZE, &id, &addr);
	if (ret < 0) {
		fprintf(stderr, "error: can't unpack IPC message from SW \n");
		return;
	}

	ret = fw_stream_ipc_handler(id, data, size);
	if (ret < 0) {
		fprintf(stderr, "error: IPC message failed\n");
		return;
	}
}

/* FW receives new IPC from SW */
void test_send_ipc_to_sw(void *data, int size)
{
	char unpacked_data[IPC_MAX_MSG_SIZE];	/* local buffer */
	uint32_t id, addr;
	int ret;

	ret = ipct_msg_unpack(data, size, unpacked_data,
			      IPC_MAX_MSG_SIZE, &id, &addr);
	if (ret < 0) {
		fprintf(stderr, "error: can't unpack IPC message from FW \n");
		return;
	}

	ret = sw_stream_ipc_handler(id, data, size);
	if (ret < 0) {
		fprintf(stderr, "error: IPC message failed\n");
		return;
	}
}

int main(int argc, char *argv[])
{
	printf("IPC Audio example\n");

	sw_run_stream_tests();

	return 0;
}
