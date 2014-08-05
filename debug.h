/* debug.h -- debug utilities
 *
 * Copyright (C) 2010,2011,2014 Olaf Bergmann <bergmann@tzi.org>
 *
 * This file is part of the CoAP library libcoap. Please see
 * README for terms of use. 
 */

#ifndef _COAP_DEBUG_H_
#define _COAP_DEBUG_H_

#include "config.h"

#ifndef COAP_DEBUG_FD
#define COAP_DEBUG_FD stdout
#endif

#ifndef COAP_ERR_FD
#define COAP_ERR_FD stderr
#endif

#include "logging.h"
#include "pdu.h"
void coap_show_pdu(const coap_pdu_t *);

struct coap_address_t;
size_t coap_print_addr(const struct coap_address_t *, unsigned char *, size_t);

#endif /* _COAP_DEBUG_H_ */
