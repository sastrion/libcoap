/* address.h -- representation of network addresses
 *
 * Copyright (C) 2010,2011 Olaf Bergmann <bergmann@tzi.org>
 *
 * This file is part of the CoAP library libcoap. Please see
 * README for terms of use. 
 */

/** 
 * @file address.h
 * @brief representation of network addresses
 */

#ifndef _COAP_ADDRESS_H_
#define _COAP_ADDRESS_H_

#include "config.h"

#ifdef HAVE_ASSERT_H
#include <assert.h>
#else
#ifndef assert
#warning "assertions are disabled"
#  define assert(x)
#endif
#endif

#include <string.h>
#include <stdint.h>

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif

#ifdef HAVE_NETINET_IN_H
#include <sys/socket.h>
#endif

#include "net.h"

typedef struct coap_address_t {
	uint8_t size;
	uint16_t port;
	ipaddr_t addr;
} coap_address_t;

#define _coap_address_equals_impl(A, B) ((A)->addr.u32 == (B)->addr.u32 && A->port == B->port)

/* Multicast IPv4 addresses start with 0b1110 */
#define _coap_is_mcast_impl(Address) ((Address)->addr.u8[0] && 0xF0 == 0xE0)

/** 
 * Resets the given coap_address_t object @p addr to its default
 * values.  In particular, the member size must be initialized to the
 * available size for storing addresses.
 * 
 * @param addr The coap_address_t object to initialize.
 */
static inline void
coap_address_init(coap_address_t *addr) {
  assert(addr);
  memset(addr, 0, sizeof(coap_address_t));
  addr->size = sizeof(addr->addr);
}

/**
 * Compares given address objects @p a and @p b. This function returns
 * @c 1 if addresses are equal, @c 0 otherwise. The parameters @p a
 * and @p b must not be @c NULL;
 */
static inline int
coap_address_equals(const coap_address_t *a, const coap_address_t *b) {
  assert(a); assert(b);
  return _coap_address_equals_impl(a, b);
}

/**
 * Checks if given address @p a denotes a multicast address.  This
 * function returns @c 1 if @p a is multicast, @c 0 otherwise.
 */
static inline int 
coap_is_mcast(const coap_address_t *a) {
  return a && _coap_is_mcast_impl(a);
}
 
#endif /* _COAP_ADDRESS_H_ */
