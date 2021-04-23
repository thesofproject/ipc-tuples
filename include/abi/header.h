/* SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright(c) 2020 Intel Corporation. All rights reserved.
 *
 */

/*
 * ABI: Mandatory header for all IPCT messages.
 */

/**
 * \file include/ipct/header.h
 * \brief IPC header
 */

#ifndef __IPCT_ABI_HEADER_H__
#define __IPCT_ABI_HEADER_H__

#include <stdint.h>

/** \addtogroup IPCT_uapi uAPI
 *  IPCT uAPI specification.
 *
 * All standard mailbox IPCT messages between host driver and DSP start
 * with a common IPC header structure. This structure is
 *
 * IPCT message header is 1 - 5 words made up as follows :
 *
 * The IPCT header works at a high level on the basic principle
 * of a mandatory header metadata followed by optional message metadata.
 *
 * +------------------------+-----------+---------+---------------+
 * | struct ipct_hdr        | Mandatory | 1 word  |               |
 * +------------------------+-----------+---------+---------------+
 * | struct ipct_route      | Optional  | 2 words | hdr.route = 1 |
 * +------------------------+-----------+---------+---------------+
 * | struct ipct_elem       | Optional  | 1 word  | hdr.elems = 1 |
 * +------------------------+-----------+---------+---------------+
 * |                                                              |
 * | Message body follows here                                    |
 * +--------------------------------------------------------------+
 *
 * The optional message metadata is ordered, i.e. it always appears in the
 * same order if used (and usage is determined by status bits in
 * struct ipct_hdr).
 *
 * The header is designed to support the following use cases.
 *
 * 1) Nano messaging via 32bit message and reply. i.e. sending header and
 *    replying with header only is enough for some use cases like starting and
 *    stopping global events.
 *
 * 2) Micro messaging via 64bit message and reply - send and reply header with
 *    micro tuple. Expands uses cases from 1) to support stopping and starting
 *    targeted events.
 *
 * 3) Variable size message and reply - like 1) and 2) but messages and replies
 *    can be variable in size from 32bits upwards. Any use case can be supported
 *    here since there are no message restrictions.
 *
 * 4) High priority messaging. The header now supports a hint for incoming
 *    message Q handlers so that they can prioritise real time high priority
 *    messages over standard batch messages. e.g stream start for low latency
 *    stream could be processed in the Q before sensor config message.
 *
 * 5) Datagram mode (no reply needed). The header can tell the message receiver
 *    that the message does not need to be acknowledged with a reply. Useful
 *    where the sender may be sending high volume, short lifetime information
 *    or where the sender does not care about reply (to save cycles on both
 *    sender and receiver). Door bell protocol would still be followed.
 *
 * 6) Message addressing. 32bit sender and receiver addresses can be added in
 *    header so that messages can be more easily routed to the correct
 *    destinations. Broadcast messages also supported.
 *
 *  @{
 */

/**
 * Structure Header - Mandatory.
 * Header metadata for all IPC commands. Identifies IPC message.
 * Class spelled with K for C++ users.
 *
 * @klass: 	Message feature class. e.g. audio, sensor, debug.
 * @subklass:	Message sub feature. e.g. PCM, kcontrol, Compressed PCM
 * @action:	Message action. e.g. Start (PCM), Get (kcontrol value)
 * @ack:	Reply - IPC success, other reply data may or may not follow.
 */
struct ipct_hdr {
	uint32_t klass : 8;		/**< class ID */
	uint32_t subklass : 8;		/**< subclass ID */
	uint32_t action : 8;		/**< action ID */
	uint32_t status : 1;		/**< status 0 is success - 1 is error */
	uint32_t priority : 1;		/**< 0 normal, 1 high */
	uint32_t datagram : 1;		/**< is datagram - no reply needed */
	uint32_t route : 1;		/**< sof_ipct_route is appended */
	uint32_t elems : 1;		/**< sof_ipct_elems is appended */
	uint32_t vendor : 3;		/**< MSBs TBD by vendor */
} __attribute__((packed, aligned(4)));

/*
 * Structure Route - Optional
 * Header routing data for this message. Allows 1:1 and 1:N messaging.
 */

#define SOF_IPCT_ROUTE_BROADCAST	0xFFFFFFFF

struct sof_ipct_route {
	uint32_t receiver;		/**< receiver ID */
	uint32_t sender;		/**< sender ID */
} __attribute__((packed, aligned(4)));

/*
 * Structure Size - Optional.
 * Header containing message size
 */
struct sof_ipct_elems {
	uint32_t num_tuples;		/**< number of data elems */
	uint32_t remaining : 8;		/**< compound chunks remaining */
	uint32_t size : 24;		/**< size in words */
} __attribute__((packed, aligned(4)));

#endif /* __IPCT_ABI_HEADER_H__ */
