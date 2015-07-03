/*
 * coap_posix.h
 *
 *  Created on: Jun 11, 2015
 *      Author: wojtek
 */

#ifndef INCLUDE_COAP_COAP_POSIX_H_
#define INCLUDE_COAP_COAP_POSIX_H_

/**
 * This data type represents internal timer ticks with COAP_TICKS_PER_SECOND
 * resolution.
 */
typedef unsigned long coap_tick_t;

/**
 * CoAP time in seconds since epoch.
 */
typedef time_t coap_time_t;

/**
 * This data type is used to represent the difference between two clock_tick_t
 * values. This data type must have the same size in memory as coap_tick_t to
 * allow wrapping.
 */
typedef long coap_tick_diff_t;

/** Use ms resolution on POSIX systems */
#define COAP_TICKS_PER_SECOND 1000

/**
 * Initializes the internal clock.
 */
static inline void coap_clock_init(void) {
}

/**
 * Sets @p t to the internal time with COAP_TICKS_PER_SECOND resolution.
 */
void coap_ticks(coap_tick_t *t);

/**
 * Helper function that converts coap ticks to wallclock time. On POSIX, this
 * function returns the number of seconds since the epoch. On other systems, it
 * may be the calculated number of seconds since last reboot or so.
 *
 * @param t Internal system ticks.
 *
 * @return  The number of seconds that has passed since a specific reference
 *          point (seconds since epoch on POSIX).
 */
static inline coap_time_t coap_ticks_to_rt(coap_tick_t t) {
  return t / COAP_TICKS_PER_SECOND;
}

#endif /* INCLUDE_COAP_COAP_POSIX_H_ */
