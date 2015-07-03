/*
 * coap_lwip_io.h
 *
 *  Created on: Jun 11, 2015
 *      Author: wojtek
 */

#ifndef PLATFORM_COAP_LWIP_IO_H_
#define PLATFORM_COAP_LWIP_IO_H_

#include <lwip/pbuf.h>
#include <lwip/udp.h>

#define CUSTOM_PDU_FIELDS  struct pbuf *pbuf;        /**< lwIP PBUF. The package data will always reside
                                                     *    inside the pbuf's payload, but this pointer
                                                     *    has to be kept because no exact offset can be
                                                     *    given. This field must not be accessed from
                                                     *    outside, because the pbuf's reference count
                                                     *    is checked to be 1 when the pbuf is assigned
                                                     *    to the pdu, and the pbuf stays exclusive to
                                                     *    this pdu. */

/*
 * This is only included in coap_io.h instead of .c in order to be available for
 * sizeof in lwippools.h.
 * Simple carry-over of the incoming pbuf that is later turned into a node.
 *
 * Source address data is currently side-banded via ip_current_dest_addr & co
 * as the packets have limited lifetime anyway.
 */
struct coap_packet_t {
  struct pbuf *pbuf;
  const coap_endpoint_t *local_interface;
  uint16_t srcport;
};

/**
 * Abstraction of virtual endpoint that can be attached to coap_context_t. The
 * tuple (handle, addr) must uniquely identify this endpoint.
 */
typedef struct coap_endpoint_t {
  struct udp_pcb *pcb;
 /**< @FIXME this was added in a hurry, not sure it confirms to the overall model --chrysn */
  struct coap_context_t *context;
  coap_address_t addr; /**< local interface address */
  int ifindex;
  int flags;
} coap_endpoint_t;

/**
 * Get the pbuf of a packet. The caller takes over responsibility for freeing
 * the pbuf.
 */
struct pbuf *coap_packet_extract_pbuf(coap_packet_t *packet);

/**
 * Creates a CoAP PDU from an lwIP @p pbuf, whose reference is passed on to this
 * function.
 *
 * The pbuf is checked for being contiguous, and for having only one reference.
 * The reference is stored in the PDU and will be freed when the PDU is freed.
 *
 * (For now, these are fatal errors; in future, a new pbuf might be allocated,
 * the data copied and the passed pbuf freed).
 *
 * This behaves like coap_pdu_init(0, 0, 0, pbuf->tot_len), and afterwards
 * copying the contents of the pbuf to the pdu.
 *
 * @return A pointer to the new PDU object or @c NULL on error.
 */
coap_pdu_t * coap_pdu_from_pbuf(struct pbuf *pbuf);

#endif /* PLATFORM_COAP_LWIP_IO_H_ */
