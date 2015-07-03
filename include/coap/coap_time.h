/* coap_time.h -- Clock Handling
 *
 * Copyright (C) 2010--2013 Olaf Bergmann <bergmann@tzi.org>
 *
 * This file is part of the CoAP library libcoap. Please see
 * README for terms of use.
 */

/**
 * @file coap_time.h
 * @brief Clock Handling
 */

#ifndef _COAP_TIME_H_
#define _COAP_TIME_H_

/*
** Make sure we can call this stuff from C++.
*/
#ifdef __cplusplus
extern "C" {
#endif

#include "coap_config.h"

/**
 * @defgroup clock Clock Handling
 * Default implementation of internal clock.
 * @{
 */

/**
 * Returns @c 1 if and only if @p a is less than @p b where less is defined on a
 * signed data type.
 */
static inline int coap_time_lt(coap_tick_t a, coap_tick_t b) {
  return ((coap_tick_diff_t)(a - b)) < 0;
}

/**
 * Returns @c 1 if and only if @p a is less than or equal @p b where less is
 * defined on a signed data type.
 */
static inline int coap_time_le(coap_tick_t a, coap_tick_t b) {
  return a == b || coap_time_lt(a,b);
}

/** @} */

#ifdef __cplusplus
}
#endif


#endif /* _COAP_TIME_H_ */
