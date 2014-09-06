/* mem.c -- CoAP memory handling
 *
 * Copyright (C) 2014 Olaf Bergmann <bergmann@tzi.org>
 *
 * This file is part of the CoAP library libcoap. Please see
 * README for terms of use.
 */

#include "ch.h"

#include "config.h"
#include "mem.h"
#include "debug.h"

#include "resource.h"

#ifndef MAX_RESOURCES
#define MAX_RESOURCES                50
#endif

#ifndef LIBCOAP_DEFAULT_HEAP_SIZE
#define LIBCOAP_DEFAULT_HEAP_SIZE    4096
#endif

static MemoryPool resource_pool;
static stkalign_t resource_buffer[MAX_RESOURCES*MEM_ALIGN_NEXT((sizeof(coap_resource_t)+TEO_URI_LENGTH+TEO_USER_DATA_LENGTH))/sizeof(stkalign_t)];

static MemoryHeap default_heap;
static stkalign_t default_heap_buffer[STACK_ALIGN(LIBCOAP_DEFAULT_HEAP_SIZE)/sizeof(stkalign_t)];

void
coap_memory_init(void) {
  chPoolInit(&resource_pool, sizeof(coap_resource_t), NULL);
  for (int i = 0; i < MAX_RESOURCES; i++) {
    chPoolFree(&resource_pool, (uint8_t *)resource_buffer + i*MEM_ALIGN_NEXT(sizeof(coap_resource_t)));
  }
  chHeapInit(&default_heap, default_heap_buffer, sizeof(default_heap_buffer));
}

void *
coap_malloc_type(coap_memory_tag_t type, size_t size) {
  switch (type) {
    case COAP_RESOURCE:
      return chPoolAlloc(&resource_pool);
    default:
      return chHeapAlloc(&default_heap, size);
    }
}

void
coap_free_type(coap_memory_tag_t type, void *object) {
  switch (type) {
    case COAP_RESOURCE:
      return chPoolFree(&resource_pool, object);
    default:
      return chHeapFree(object);
    }
}
