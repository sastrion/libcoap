/* str.c -- strings to be used in the CoAP library
 *
 * Copyright (C) 2010,2011 Olaf Bergmann <bergmann@tzi.org>
 *
 * This file is part of the CoAP library libcoap. Please see
 * README for terms of use. 
 */

#include "config.h"

#include <stdio.h>

#include "debug.h"
#include "mem.h"
#include "str.h"

DEFINE_LOG(LOG_DEFAULT_SEVERITY);

str *coap_new_string(size_t size) {
  str *s = coap_malloc(sizeof(str) + size + 1);
  if ( !s ) {
    error("coap_new_string: malloc\n");
    return NULL;
  }

  memset(s, 0, sizeof(str));
  s->s = ((unsigned char *)s) + sizeof(str);
  return s;
}

void coap_delete_string(str *s) {
  coap_free(s);
}

