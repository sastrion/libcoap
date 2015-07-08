/* address.h -- representation of network addresses
 *
 * Copyright (C) 2010,2011,2015 Olaf Bergmann <bergmann@tzi.org>
 *
 * This file is part of the CoAP library libcoap. Please see
 * README for terms of use.
 */

/**
 * @file address.h
 * @brief Representation of network addresses
 */

#ifndef _COAP_ADDRESS_H_
#define _COAP_ADDRESS_H_

#include "coap_config.h"

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

/**
 * Resets the given coap_address_t object @p addr to its default values. In
 * particular, the member size must be initialized to the available size for
 * storing addresses.
 *
 * @param addr The coap_address_t object to initialize.
 */
static inline void
coap_address_init(coap_address_t *addr) {
  assert(addr);
  memset(addr, 0, sizeof(coap_address_t));
#ifdef WITH_POSIX
  /* lwip and Contiki have constant address sizes and doesn't need the .size part */
  addr->size = sizeof(addr->addr);
#endif
}

/**
 * Compares given address objects @p a and @p b. This function returns @c 1 if
 * addresses are equal, @c 0 otherwise. The parameters @p a and @p b must not be
 * @c NULL;
 */
static inline int
coap_address_equals(const coap_address_t *a, const coap_address_t *b) {
  assert(a); assert(b);
  return _coap_address_equals_impl(a, b);
}

/**
 * Copy given address from @p src to @p dest.
 */
static inline void
coap_address_copy(coap_address_t *dest, const coap_address_t *src) {
	(void)dest;
	(void)src;
}

/**
 * Checks if given address object @p a denotes the wildcard address. This
 * function returns @c 1 if this is the case, @c 0 otherwise. The parameters @p
 * a must not be @c NULL;
 */
static inline int
coap_address_isany(const coap_address_t *a) {
  assert(a);
  return _coap_address_isany_impl(a);
}

/**
 * Checks if given address @p a denotes a multicast address. This function
 * returns @c 1 if @p a is multicast, @c 0 otherwise.
 */
static inline int
coap_is_mcast(const coap_address_t *a) {
  return a && _coap_is_mcast_impl(a);
}

/**
 * Fill an address structure with given address and port.
 *
 */
static inline int
coap_address(const char *address, unsigned short port, coap_address_t *result) {
  assert(result);
  return _coap_address_impl(address, port, result);
}
#endif /* _COAP_ADDRESS_H_ */
