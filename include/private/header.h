/* SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright(c) 2020 Intel Corporation. All rights reserved.
 *
 * PRIVATE HEADER INTERNAL USE ONLY.
 */

/**
 * \file include/ipct/header.h
 * \brief IPC header
 */

#ifndef __IPCT_PRIVATE_HEADER_H__
#define __IPCT_PRIVATE_HEADER_H__

#include <stdint.h>
#include <abi/header.h>

/*
 * Convenience macros to get struct offsets from header.
 */

/* Internal macros to get header ponters. */
#define IPCT_HDR_ROUTE_ADD(hdr)	\
	(hdr->route ? sizeof(struct sof_ipct_route) : 0)
#define IPCT_HDR_ELEM_ADD(hdr) 	\
	(hdr->elems ? sizeof(struct sof_ipct_elems) : 0)

/* get header route pointer or NULL */
#define IPCT_HDR_GET_ROUTE_PTR(hdr) 	\
	(hdr->route ? (void *)hdr +		\
		sizeof(struct ipct_hdr) : 0)

/* get header elem pointer or NULL */
#define IPCT_HDR_GET_ELEM_PTR(hdr) 		\
	(hdr->elems ? (void *)hdr +  sizeof(struct ipct_hdr) +	\
		IPCT_HDR_ROUTE_ADD(hdr) : 0)

/* get headers size */
#define IPCT_HDR_GET_HDR_SIZE(hdr) 		\
		sizeof(struct ipct_hdr) +	\
		IPCT_HDR_ELEM_ADD(hdr) +	\
		IPCT_HDR_ROUTE_ADD(hdr)

/* get first tuple */
#define IPCT_HDR_GET_TUPLE(hdr)			\
	((void*)(hdr) + IPCT_HDR_GET_HDR_SIZE(hdr))

/* get ID from header */
#define IPCT_HDR_GET_ID(hdr)			\
	(*((uint32_t*)(hdr)) & 0x00ffffff)

/* ID helpers */
#define IPCT_ID_GET_KLASS(id)		(((id) & 0x000000FF) >> 0)
#define IPCT_ID_GET_SUBKLASS(id)	(((id) & 0x0000FF00) >> 8)
#define IPCT_ID_GET_ACTION(id)		(((id) & 0x00FF0000) >> 16)

/* helpers */
static inline uint32_t ipct_get_tuples(struct ipct_hdr *hdr)
{
	struct sof_ipct_elems *e;

	e = IPCT_HDR_GET_ELEM_PTR(hdr);

	return e ? e->num_tuples : 0;
}

static inline uint32_t ipct_get_size(struct ipct_hdr *hdr)
{
	struct sof_ipct_elems *e;

	e = IPCT_HDR_GET_ELEM_PTR(hdr);

	return e ? e->size * sizeof(uint32_t) : 0;
}

static inline uint32_t ipct_get_remaining(struct ipct_hdr *hdr)
{
	struct sof_ipct_elems *e;

	e = IPCT_HDR_GET_ELEM_PTR(hdr);

	return e ? e->remaining : 0;
}

static inline uint32_t ipct_get_receiver(struct ipct_hdr *hdr)
{
	struct sof_ipct_route *r;

	r = IPCT_HDR_GET_ROUTE_PTR(hdr);

	return r ? r->receiver : 0;
}

static inline uint32_t ipct_get_sender(struct ipct_hdr *hdr)
{
	struct sof_ipct_route *r;

	r = IPCT_HDR_GET_ROUTE_PTR(hdr);

	return r ? r->sender : 0;
}

/** @} */

#endif /* __IPCT_PRIVATE_HEADER_H__ */
