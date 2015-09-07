#include "coap_config.h"

#include <ctype.h>
#include <stdio.h>
#include <errno.h>
#ifdef HAVE_LIMITS_H
#include <limits.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#elif HAVE_SYS_UNISTD_H
#include <sys/unistd.h>
#endif
#include <sys/types.h>
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif

#include "debug.h"
#include "mem.h"
#include "str.h"
#include "async.h"
#include "resource.h"
#include "option.h"
#include "encode.h"
#include "block.h"
#include "net.h"
#include "coap_timer.h"

#if defined(WITH_POSIX)

time_t clock_offset;

#elif defined(WITH_CONTIKI)

clock_time_t clock_offset;

#endif /* WITH_CONTIKI */


#ifdef WITH_CONTIKI // TODO Should be more abstracted. E.g. CONTEXT_SINGLETON
unsigned char initialized = 0;
coap_context_t the_coap_context;
#endif

coap_context_t *
coap_new_context(
  const coap_address_t *listen_addr) {
#ifndef WITH_CONTIKI
  coap_context_t *c = coap_malloc_type(COAP_CONTEXT, sizeof( coap_context_t ) );
#endif /* not WITH_CONTIKI */
#ifdef WITH_CONTIKI
  coap_context_t *c;

  if (initialized)
    return NULL;
#endif /* WITH_CONTIKI */

  if (!listen_addr) {
    coap_log(LOG_EMERG, "no listen address specified\n");
    return NULL;
  }

  coap_clock_init();
#ifdef WITH_LWIP
  prng_init(LWIP_RAND());
#endif /* WITH_LWIP */
#ifdef WITH_CONTIKI
  prng_init((ptrdiff_t)listen_addr ^ clock_offset);
#endif /* WITH_LWIP */
#ifdef WITH_POSIX
  prng_init((unsigned long)listen_addr ^ clock_offset);
#endif /* WITH_POSIX */

#ifndef WITH_CONTIKI
  if (!c) {
#ifndef NDEBUG
    coap_log(LOG_EMERG, "coap_init: malloc:\n");
#endif
    return NULL;
  }
#endif /* not WITH_CONTIKI */
#ifdef WITH_CONTIKI
  coap_resources_init();
  coap_memory_init();

  c = &the_coap_context;
  initialized = 1;
#endif /* WITH_CONTIKI */

  memset(c, 0, sizeof( coap_context_t ) );

  /* initialize message id */
  prng((unsigned char *)&c->message_id, sizeof(unsigned short));

  /* register the critical options that we know */
  coap_register_option(c, COAP_OPTION_IF_MATCH);
  coap_register_option(c, COAP_OPTION_URI_HOST);
  coap_register_option(c, COAP_OPTION_IF_NONE_MATCH);
  coap_register_option(c, COAP_OPTION_URI_PORT);
  coap_register_option(c, COAP_OPTION_URI_PATH);
  coap_register_option(c, COAP_OPTION_URI_QUERY);
  coap_register_option(c, COAP_OPTION_ACCEPT);
  coap_register_option(c, COAP_OPTION_PROXY_URI);
  coap_register_option(c, COAP_OPTION_PROXY_SCHEME);
  coap_register_option(c, COAP_OPTION_BLOCK2);
  coap_register_option(c, COAP_OPTION_BLOCK1);

  c->endpoint = coap_new_endpoint(listen_addr, COAP_ENDPOINT_NOSEC);
  if (c->endpoint == NULL) {
    goto onerror;
  }

#ifdef WITH_POSIX
  c->sockfd = c->endpoint->handle.fd;
#endif /* WITH_POSIX */

  c->network_send = coap_network_send;
  c->network_read = coap_network_read;

# ifndef WITHOUT_OBSERVE
  c->notify_timer = coap_new_timer(NULL, c); //FIXME
  coap_timer_set(c->notify_timer, COAP_RESOURCE_CHECK_TIME * COAP_TICKS_PER_SECOND);
#endif /* WITHOUT_OBSERVE */

  /* the retransmit timer must be initialized to some large value */
  c->retransmit_timer = coap_new_timer(NULL, c); //FIXME
  coap_timer_set(c->retransmit_timer, 0xFFFF);

  return c;

 onerror:
  coap_free(c);
  return NULL;
}

void
coap_free_context(coap_context_t *context) {

  if (!context)
    return;

  coap_delete_all(context->sendqueue);

  coap_free_timer(context->retransmit_timer);
#ifndef WITHOUT_OBSERVE
  coap_free_timer(context->notify_timer);
#endif

  coap_delete_all_resources(context);

  coap_free_endpoint(context->endpoint);
#ifdef WITH_CONTIKI
  memset(&the_coap_context, 0, sizeof(coap_context_t));
  initialized = 0;
#else
  coap_free_type(COAP_CONTEXT, context);
#endif /* WITH_CONTIKI */
}

