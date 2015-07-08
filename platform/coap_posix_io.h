/*
 * coap_posix.io.h
 *
 *  Created on: Jun 11, 2015
 *      Author: wojtek
 */

#ifndef _COAP_POSIX_IO_H_
#define _COAP_POSIX_IO_H_

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif

#ifdef HAVE_NETINET_IN_H
#include <sys/socket.h>
#endif

#ifdef HAVE_ASSERT_H
#include <assert.h>
#endif

#include <coap/coap_io.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>

#define CUSTOM_CONTEXT_FIELDS \
  int sockfd;                     /**< send/receive socket */

/**
 * Abstract handle that is used to identify a local network interface.
 */
typedef int coap_if_handle_t;


/** multi-purpose address abstraction */
typedef struct coap_address_t {
  socklen_t size;           /**< size of addr */
  union {
    struct sockaddr         sa;
    struct sockaddr_storage st;
    struct sockaddr_in      sin;
    struct sockaddr_in6     sin6;
  } addr;
} coap_address_t;

static inline int
_coap_address_impl(const char *address, unsigned short port, coap_address_t *result)
{
  struct addrinfo *res, *ainfo;
  struct addrinfo hints;
  int error, len = -1;

  memset((char *)result, 0, sizeof(coap_address_t));
  memset((char *)&hints, 0, sizeof(hints));
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_family = AF_UNSPEC;
  hints.ai_flags = AI_NUMERICHOST | AI_PASSIVE;

  error = getaddrinfo(address, NULL, &hints, &res);

  if (error != 0) {
    return error;
  }

  for (ainfo = res; ainfo != NULL; ainfo = ainfo->ai_next) {
    switch (ainfo->ai_family) {
      case AF_INET6:
      case AF_INET:
        result->size = ainfo->ai_addrlen;
        memcpy(&result->addr, ainfo->ai_addr, result->size);
        if (ainfo->ai_family == AF_INET)
          result->addr.sin.sin_port = htons(port);
        else
          result->addr.sin6.sin6_port = htons(port);
        goto finish;
      default:
        ;
    }
  }

finish:
  freeaddrinfo(res);
  return len;
}

static inline int
_coap_address_equals_impl(const coap_address_t *a, const coap_address_t *b) {
  assert(a); assert(b);

  if (a->size != b->size || a->addr.sa.sa_family != b->addr.sa.sa_family)
    return 0;

  /* need to compare only relevant parts of sockaddr_in6 */
 switch (a->addr.sa.sa_family) {
 case AF_INET:
   return
     a->addr.sin.sin_port == b->addr.sin.sin_port &&
     memcmp(&a->addr.sin.sin_addr, &b->addr.sin.sin_addr,
        sizeof(struct in_addr)) == 0;
 case AF_INET6:
   return a->addr.sin6.sin6_port == b->addr.sin6.sin6_port &&
     memcmp(&a->addr.sin6.sin6_addr, &b->addr.sin6.sin6_addr,
        sizeof(struct in6_addr)) == 0;
 default: /* fall through and signal error */
   ;
 }
 return 0;
}

static inline int
_coap_address_isany_impl(const coap_address_t *a) {
  /* need to compare only relevant parts of sockaddr_in6 */
  switch (a->addr.sa.sa_family) {
  case AF_INET:
    return a->addr.sin.sin_addr.s_addr == INADDR_ANY;
  case AF_INET6:
    return memcmp(&in6addr_any,
                  &a->addr.sin6.sin6_addr,
                  sizeof(in6addr_any)) == 0;
  default:
    ;
  }

  return 0;
}

static inline int
_coap_is_mcast_impl(const coap_address_t *a) {
  if (!a)
    return 0;

 switch (a->addr.sa.sa_family) {
 case AF_INET:
   return IN_MULTICAST(a->addr.sin.sin_addr.s_addr);
 case  AF_INET6:
   return IN6_IS_ADDR_MULTICAST(&a->addr.sin6.sin6_addr);
 default:  /* fall through and signal error */
   ;
  }
 return 0;
}

/**
 * Abstraction of virtual endpoint that can be attached to coap_context_t. The
 * tuple (handle, addr) must uniquely identify this endpoint.
 */
typedef struct coap_endpoint_t {
  int fd;       /**< on POSIX systems */
  coap_address_t addr; /**< local interface address */
  int ifindex;
  int flags;
  coap_network_read_t network_read;
  coap_network_send_t network_send;
} coap_endpoint_t;

struct coap_packet_t {
  coap_if_handle_t hnd;	      /**< the interface handle */
  coap_address_t src;	      /**< the packet's source address */
  coap_address_t dst;	      /**< the packet's destination address */
  const coap_endpoint_t *interface;

  int ifindex;
  void *session;		/**< opaque session data */

  size_t length;		/**< length of payload */
  unsigned char payload[];	/**< payload */
};

#endif /* _COAP_POSIX_IO_H_ */
