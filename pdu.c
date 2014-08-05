/* pdu.c -- CoAP message structure
 *
 * Copyright (C) 2010,2011 Olaf Bergmann <bergmann@tzi.org>
 *
 * This file is part of the CoAP library libcoap. Please see
 * README for terms of use. 
 */

#include "config.h"

#include "logging.h"
DEFINE_LOG(LOG_DEFAULT_SEVERITY);

#if defined(HAVE_ASSERT_H) && !defined(assert)
# include <assert.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif

#include "debug.h"
#include "pdu.h"
#include "option.h"
#include "encode.h"

#include "mem.h"


void
coap_pdu_clear(coap_pdu_t *pdu, size_t size) {
  assert(pdu);

  memset(pdu, 0, sizeof(coap_pdu_t)); /* MWAS: for st-node payload is in separate memory area */
  pdu->max_size = size;

  /* data is NULL unless explicitly set by coap_add_data() */
  pdu->length = sizeof(coap_hdr_t);
}

#include "net.h"
#include "system.h"

coap_pdu_t *
coap_pdu_from_mbuf(struct mbuf *mbuf)
{
  coap_pdu_t *result;
  //TODO: MWAS: get rid of sys_malloc
  result = sys_malloc(sizeof(coap_pdu_t));

  memset(result, 0, sizeof(coap_pdu_t));

  result->max_size = mbuf->tot_len;
  result->length = mbuf->tot_len;
  result->hdr = (coap_hdr_t *)((unsigned char *)mbuf->payload);
  result->mbuf = mbuf;

  return result;
}

coap_pdu_t *
coap_pdu_init(unsigned char type, unsigned char code, 
	      unsigned short id, size_t size) {
  coap_pdu_t *pdu;

    struct mbuf *m;


  assert(size <= COAP_MAX_PDU_SIZE);
  /* Size must be large enough to fit the header. */
  if (size < sizeof(coap_hdr_t) || size > COAP_MAX_PDU_SIZE)
    return NULL;

  //TODO: MWAS: get rid of sys_malloc
  pdu = sys_malloc(sizeof(coap_pdu_t));
  m = mbuf_new();

  if (pdu) {
    coap_pdu_clear(pdu, size);
    pdu->hdr = (coap_hdr_t *)((unsigned char *)m->payload);
    pdu->hdr->version = COAP_DEFAULT_VERSION;
    pdu->hdr->token_length = 0;
    pdu->mbuf = m;
    pdu->mbuf->len = sizeof(coap_hdr_t);
    pdu->mbuf->tot_len = sizeof(coap_hdr_t);
    pdu->hdr->id = id;
    pdu->hdr->type = type;
    pdu->hdr->code = code;
  } 
  return pdu;
}

coap_pdu_t *
coap_new_pdu(void) {
  coap_pdu_t *pdu;

  pdu = coap_pdu_init(0, 0, ntohs(COAP_INVALID_TID), COAP_MAX_PDU_SIZE);
  if (!pdu)
    error("coap_new_pdu: cannot allocate memory for new PDU\n");

  return pdu;
}

void
coap_delete_pdu(coap_pdu_t *pdu) {

  if (pdu != NULL) {
    mbuf_free(pdu->mbuf);
    sys_free(pdu);
  }
}

int
coap_add_token(coap_pdu_t *pdu, size_t len, const unsigned char *data) {

  const size_t HEADERLENGTH = len + 4;
  /* must allow for pdu == NULL as callers may rely on this */
  if (!pdu || len > 8 || pdu->max_size < HEADERLENGTH)
    return 0;

  pdu->hdr->token_length = len;
  if (len)
    mbuf_write(pdu->mbuf, data, len, pdu->length);

  pdu->max_delta = 0;
  pdu->length = HEADERLENGTH;
  pdu->data = NULL;

  return 1;
}

/** @FIXME de-duplicate code with coap_add_option_later */
size_t
coap_add_option(coap_pdu_t *pdu, unsigned short type, unsigned int len, const unsigned char *data) {
  size_t optsize;

  assert(pdu);
  pdu->data = NULL;

  if (type < pdu->max_delta) {
    warn("coap_add_option: options are not in correct order\n");
    return 0;
  }

  /* encode option and check length */
  optsize = coap_opt_encode_to_mbuf(pdu, type, data, len);

  if (!optsize) {
    warn("coap_add_option: cannot add option\n");
    /* error */
    return 0;
  } else {
    pdu->max_delta = type;
    pdu->length += optsize;
  }

  return optsize;
}

/** @FIXME de-duplicate code with coap_add_option */
unsigned char*
coap_add_option_later(coap_pdu_t *pdu, unsigned short type, unsigned int len) {
  size_t optsize;
  coap_opt_t *opt;

  assert(pdu);
  pdu->data = NULL;

  if (type < pdu->max_delta) {
    warn("coap_add_option: options are not in correct order\n");
    return NULL;
  }

  opt = (unsigned char *)pdu->hdr + pdu->length;

  /* encode option and check length */
  optsize = coap_opt_encode_to_mbuf(pdu, type, NULL, len);

  if (!optsize) {
    warn("coap_add_option: cannot add option\n");
    /* error */
    return NULL;
  } else {
    pdu->max_delta = type;
    pdu->length += optsize;
  }

  return ((unsigned char*)opt) + optsize - len;
}

int
coap_add_data(coap_pdu_t *pdu, unsigned int len, const unsigned char *data) {
  assert(pdu);
  assert(pdu->data == NULL);

  if (len == 0)
    return 1;

  if (pdu->length + len + 1 > pdu->max_size) {
    warn("coap_add_data: cannot add: data too large for PDU\n");
    assert(pdu->data == NULL);
    return 0;
  }

  pdu->data = (unsigned char *)pdu->hdr + pdu->length;
  *pdu->data = COAP_PAYLOAD_START;
  pdu->data++;
  mbuf_write(pdu->mbuf, data, len, pdu->length+1);
  pdu->length += len + 1;
  return 1;
}

int
coap_get_data(coap_pdu_t *pdu, size_t *len, unsigned char **data) {
  assert(pdu);
  assert(len);
  assert(data);

  if (pdu->data) {
    *len = (unsigned char *)pdu->hdr + pdu->length - pdu->data;
    *data = pdu->data;
  } else {			/* no data, clear everything */
    *len = 0;
    *data = NULL;
  }

  return *data != NULL;
}

#ifndef SHORT_ERROR_RESPONSE
typedef struct {
  unsigned char code;
  char *phrase;
} error_desc_t;

/* if you change anything here, make sure, that the longest string does not 
 * exceed COAP_ERROR_PHRASE_LENGTH. */
error_desc_t coap_error[] = {
  { COAP_RESPONSE_CODE(65),  "2.01 Created" },
  { COAP_RESPONSE_CODE(66),  "2.02 Deleted" },
  { COAP_RESPONSE_CODE(67),  "2.03 Valid" },
  { COAP_RESPONSE_CODE(68),  "2.04 Changed" },
  { COAP_RESPONSE_CODE(69),  "2.05 Content" },
  { COAP_RESPONSE_CODE(400), "Bad Request" },
  { COAP_RESPONSE_CODE(401), "Unauthorized" },
  { COAP_RESPONSE_CODE(402), "Bad Option" },
  { COAP_RESPONSE_CODE(403), "Forbidden" },
  { COAP_RESPONSE_CODE(404), "Not Found" },
  { COAP_RESPONSE_CODE(405), "Method Not Allowed" },
  { COAP_RESPONSE_CODE(408), "Request Entity Incomplete" },
  { COAP_RESPONSE_CODE(413), "Request Entity Too Large" },
  { COAP_RESPONSE_CODE(415), "Unsupported Media Type" },
  { COAP_RESPONSE_CODE(500), "Internal Server Error" },
  { COAP_RESPONSE_CODE(501), "Not Implemented" },
  { COAP_RESPONSE_CODE(502), "Bad Gateway" },
  { COAP_RESPONSE_CODE(503), "Service Unavailable" },
  { COAP_RESPONSE_CODE(504), "Gateway Timeout" },
  { COAP_RESPONSE_CODE(505), "Proxying Not Supported" },
  { 0, NULL }			/* end marker */
};

char *
coap_response_phrase(unsigned char code) {
  int i;
  for (i = 0; coap_error[i].code; ++i) {
    if (coap_error[i].code == code)
      return coap_error[i].phrase;
  }
  return NULL;
}
#endif

/**
 * Advances *optp to next option if still in PDU. This function 
 * returns the number of bytes opt has been advanced or @c 0
 * on error.
 */
static size_t
next_option_safe(coap_opt_t **optp, size_t *length) {
  coap_option_t option;
  size_t optsize;

  assert(optp); assert(*optp); 
  assert(length);

  optsize = coap_opt_parse(*optp, *length, &option);
  if (optsize) {
    assert(optsize <= *length);

    *optp += optsize;
    *length -= optsize;
  }

  return optsize;
}

int
coap_pdu_parse(unsigned char *data, size_t length, coap_pdu_t *pdu) {
  coap_opt_t *opt;

  assert(data);
  assert(pdu);

  if (pdu->max_size < length) {
    debug("insufficient space to store parsed PDU\n");
    return 0;
  }

  if (length < sizeof(coap_hdr_t)) {
    debug("discarded invalid PDU\n");
  }

  pdu->hdr->version = data[0] >> 6;
  pdu->hdr->type = (data[0] >> 4) & 0x03;
  pdu->hdr->token_length = data[0] & 0x0f;
  pdu->hdr->code = data[1];
  pdu->data = NULL;

  /* sanity checks */
  if (pdu->hdr->code == 0) {
    if (length != sizeof(coap_hdr_t) || pdu->hdr->token_length) {
      debug("coap_pdu_parse: empty message is not empty\n");
      goto discard;
    }
  }

  if (length < sizeof(coap_hdr_t) + pdu->hdr->token_length
      || pdu->hdr->token_length > 8) {
    debug("coap_pdu_parse: invalid Token\n");
    goto discard;
  }

  /* Copy message id in network byte order, so we can easily write the
   * response back to the network. */
  memcpy(&pdu->hdr->id, data + 2, 2);

  /* append data (including the Token) to pdu structure */
  memcpy(pdu->hdr + 1, data + sizeof(coap_hdr_t), length - sizeof(coap_hdr_t));
  pdu->length = length;
  
  /* Finally calculate beginning of data block and thereby check integrity
   * of the PDU structure. */

  /* skip header + token */
  length -= (pdu->hdr->token_length + sizeof(coap_hdr_t));
  opt = (unsigned char *)(pdu->hdr + 1) + pdu->hdr->token_length;

  while (length && *opt != COAP_PAYLOAD_START) {

    if (!next_option_safe(&opt, (size_t *)&length)) {
      debug("coap_pdu_parse: drop\n");
      goto discard;
    }
  }

  /* end of packet or start marker */
  if (length) {
    assert(*opt == COAP_PAYLOAD_START);
    opt++; length--;

    if (!length) {
      debug("coap_pdu_parse: message ending in payload start marker\n");
      goto discard;
    }

    debug("set data to %p (pdu ends at %p)\n", (unsigned char *)opt, 
	  (unsigned char *)pdu->hdr + pdu->length);
    pdu->data = (unsigned char *)opt;
  }

  return 1;

 discard:
  return 0;
}
