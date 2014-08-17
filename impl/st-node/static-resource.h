/*
 * static-resource.h
 *
 * Based on st-node src/rest/resource.h
 *
 *  Created on: Sep 25, 2013
 *      Author: wojtek
 */

#ifndef RESOURCE_H_
#define RESOURCE_H_

#include "resource.h"

//struct rest_resource;
//struct teo_msg;
//struct teo_ctx;

typedef struct resource_list {
	slist_t resources;
} resource_list_t;

//typedef struct rest_resource {
//	unsigned int dynamic : 1;
//	char *uri;
//	void *pdata;
//	rest_handler_t handler[4];
//	slist_element_t list;
//} rest_resource_t;

#define RESOURCE(_uri, _get, _post, _put, _delete) { .uri.s = (unsigned char*)_uri, .handler = {_get, _post, _put, _delete} }

void* resource_malloc(size_t size);
void resource_free(void *p);

coap_resource_t* rest_create_resource(const char *uri, void *pdata, coap_method_handler_t (*handlers)[4]);

void rest_add_resource(resource_list_t *lst, coap_resource_t *resource);
void rest_remove_resource(resource_list_t *lst, const char *pattern);

void rest_init(resource_list_t *lst);
void rest_deinit(resource_list_t *lst);


#endif /* RESOURCE_H_ */
