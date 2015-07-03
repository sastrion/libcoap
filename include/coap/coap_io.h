/* coap_io.h -- Default network I/O functions for libcoap
 *
 * Copyright (C) 2012--2013 Olaf Bergmann <bergmann@tzi.org>
 *
 * This file is part of the CoAP library libcoap. Please see
 * README for terms of use.
 */

#ifndef _COAP_IO_H_
#define _COAP_IO_H_

#include "coap_config.h"

#ifdef HAVE_ASSERT_H
#include <assert.h>
#else
#ifndef assert
#warning "assertions are disabled"
#  define assert(x)
#endif
#endif

#ifdef HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif

#include "address.h"

/** Invalid interface handle */
#define COAP_IF_INVALID -1

#define COAP_ENDPOINT_NOSEC 0x00
#define COAP_ENDPOINT_DTLS  0x01

struct coap_context_t;
struct coap_packet_t;

/**
 * Abstract handle that is used to identify a local network interface.
 */
typedef int coap_if_handle_t;

typedef struct coap_packet_t coap_packet_t;

coap_endpoint_t *coap_new_endpoint(const coap_address_t *addr, int flags);

void coap_free_endpoint(coap_endpoint_t *ep);

/**
 * Function interface for data transmission. This function returns the number of
 * bytes that have been transmitted, or a value less than zero on error.
 *
 * @param context          The calling CoAP context.
 * @param local_interface  The local interface to send the data.
 * @param dst              The address of the receiver.
 * @param data             The data to send.
 * @param datalen          The actual length of @p data.
 *
 * @return                 The number of bytes written on success, or a value
 *                         less than zero on error.
 */
ssize_t coap_network_send(struct coap_context_t *context,
                          const coap_endpoint_t *local_interface,
                          const coap_address_t *dst,
                          unsigned char *data, size_t datalen);

/**
 * Function interface for reading data. This function returns the number of
 * bytes that have been read, or a value less than zero on error. In case of an
 * error, @p *packet is set to NULL.
 *
 * @param ep     The endpoint that is used for reading data from the network.
 * @param packet A result parameter where a pointer to the received packet
 *               structure is stored. The caller must call coap_free_packet to
 *               release the storage used by this packet.
 *
 * @return       The number of bytes received on success, or a value less than
 *               zero on error.
 */
ssize_t coap_network_read(coap_endpoint_t *ep, coap_packet_t **packet);

/**
 * Function interface for checking data availability.
 *
 * @param ep The endpoint that should be checked.
 * @return The number of bytes present in @p ep receive queue.
 */
ssize_t coap_network_peek(coap_endpoint_t *ep);

#ifndef coap_mcast_interface
# define coap_mcast_interface(Local) 0
#endif

/** Allocates store for a packet */
coap_packet_t *coap_malloc_packet(void);

/**
 * Releases the storage allocated for @p packet.
 */
void coap_free_packet(coap_packet_t *packet);

/**
 * Populate the coap_endpoint_t *target from the incoming packet's destination
 * data.
 *
 * This is usually used to copy a packet's data into a node's local_if member.
 */
void coap_packet_populate_endpoint(coap_packet_t *packet,
                                   coap_endpoint_t *target);

/**
 * Given an incoming packet, copy its source address into an address struct.
 */
void coap_packet_copy_source(coap_packet_t *packet, coap_address_t *target);

/**
 * Given a packet, set msg and msg_len to an address and length of the packet's
 * data in memory.
 * */
void coap_packet_get_memmapped(coap_packet_t *packet,
                               unsigned char **address,
                               size_t *length);

#endif /* _COAP_IO_H_ */
