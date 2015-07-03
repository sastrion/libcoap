/* mem.c -- CoAP memory handling
 *
 * Copyright (C) 2014--2015 Olaf Bergmann <bergmann@tzi.org>
 *
 * This file is part of the CoAP library libcoap. Please see
 * README for terms of use.
 */


#include "coap_config.h"
#include "mem.h"
#include "debug.h"

#ifdef HAVE_ASSERT_H
#include <assert.h>
#else /* HAVE_ASSERT_H */
#define assert(...)
#endif /* HAVE_ASSERT_H */

#ifdef HAVE_MALLOC
#include <stdlib.h>

void
coap_memory_init(void) {
}

#ifdef __GNUC__
#define UNUSED_PARAM __attribute__((unused))
#else
#define UNUSED_PARAM
#endif /* __GNUC__ */

void *
coap_malloc_type(coap_memory_tag_t type UNUSED_PARAM, size_t size) {
  return malloc(size);
}

void
coap_free_type(coap_memory_tag_t type UNUSED_PARAM, void *p) {
  free(p);
}

#endif /* HAVE_MALLOC */
