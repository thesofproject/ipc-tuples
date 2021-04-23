/* SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright(c) 2020 Intel Corporation. All rights reserved.
 *
 * Author: Liam Girdwood <liam.r.girdwood@linux.intel.com>
 */

#ifndef SRC_INCLUDE_EXAMPLE_TEST_H_
#define SRC_INCLUDE_EXAMPLE_TEST_H_

#include <stdint.h>
#include <stdlib.h>

void test_send_ipc_to_fw(void *data, int size);
void test_send_ipc_to_sw(void *data, int size);

int sw_run_stream_tests(void);
int sw_stream_ipc_handler(uint32_t id, void *structure, uint32_t size);
int fw_stream_ipc_handler(uint32_t id, void *structure, uint32_t size);

#endif
