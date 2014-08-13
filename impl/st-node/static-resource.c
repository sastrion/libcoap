#include <stdlib.h>
#include <string.h>
#include "ch.h"
#include "hal.h"
#include "logging.h"
#include "utils.h"
#include "list.h"
#include "resource.h"
#include "static-resource.h"

DEFINE_LOG(LOG_DEFAULT_SEVERITY);

#ifndef RESOURCE_HEAP_SIZE
#define RESOURCE_HEAP_SIZE	     4096
#endif

static MemoryHeap resource_heap;
static stkalign_t resource_heap_buffer[STACK_ALIGN(RESOURCE_HEAP_SIZE)/sizeof(stkalign_t)];

static uint16_t refcnt = 0;

/**
 * @brief Allocate memory on resource heap
 * @param[in] size of memory block to be allocated in bytes
 *
 */
inline void* resource_malloc(size_t size)
{
	void* p = NULL;
	p = chHeapAlloc(&resource_heap, size);
	if (p) {
		trace("A[p=%p, size=%d, refcnt=%d]", p, size, refcnt++);
		memset(p, 0, size);
	} else {
		warn("Failed to allocate [%d] bytes", size);
	}
	return p;
}

/**
 * @brief Release previously allocated memory
 *
 * @param[in]	pointer to a memory block which should be released
 */
inline void resource_free(void *p)
{
	trace("F[p=%p, refcnt=%d]", p, --refcnt);
	chHeapFree(p);
}

/**
 * @brief Delete resource.
 */
void delete_resource(coap_resource_t *r)
{
	resource_free(r);
}

/**
 * @brief Create resource.
 */
coap_resource_t* rest_create_resource(const char *uri, void *pdata, coap_method_handler_t (*handlers)[4])
{
	uint8_t *p = (uint8_t *)resource_malloc(sizeof(coap_resource_t) + strlen(uri) + 1);
	if (!p) {
		return NULL;
	}

	coap_resource_t *r = (coap_resource_t *)p;
	memcpy(r->handler, handlers, sizeof(r->handler));
	r->pdata = pdata;
	r->dynamic = 1;
	r->uri = (char *)(p + sizeof(coap_resource_t));
	strcpy(r->uri, uri);

	return r;
}

/**
 * @brief Add resource to resource database.
 *
 */
void rest_add_resource(resource_list_t *lst, coap_resource_t *resource)
{
	slist_append(&lst->resources, &resource->list);
	info("Added resource [%s]", resource->uri);
}

/**
 * @brief Remove resource by URI.
 */
void rest_remove_resource(resource_list_t *lst, const char *pattern)
{
	slist_it_t it;
	slist_for_each(&lst->resources, it) {
		coap_resource_t *resource = slist_entry(it.q, coap_resource_t, list);
		if (pattern && !strstr(resource->uri, pattern))
			continue;
		slist_delete(&lst->resources, it.p, it.q);
		if (resource->dynamic) {
			resource_free(resource);
		}
		info("Removed resource [%s]", resource->uri);
	}
}

/**
 * Init resources database.
 *
 */
void rest_init(resource_list_t *lst)
{
	chHeapInit(&resource_heap, resource_heap_buffer, sizeof(resource_heap_buffer));
	lst->resources.size = 0;
	lst->resources.head.next = &(lst->resources.head);
	lst->resources.tail = &(lst->resources.head);
}

/**
 * Deinit resources database.
 */
void rest_deinit(resource_list_t *lst)
{
	rest_remove_resource(lst, NULL);
}
