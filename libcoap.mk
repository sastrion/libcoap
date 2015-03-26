get-cwd = $(patsubst %/,%,$(dir $(abspath $(lastword $(MAKEFILE_LIST)))))
ROOT := $(call get-cwd)

# List of all the board related files.
LIBSRC += $(ROOT)/src/pdu.c \
          $(ROOT)/src/coap_net.c \
          $(ROOT)/src/debug.c \
          $(ROOT)/src/encode.c \
          $(ROOT)/src/uri.c \
          $(ROOT)/src/resource.c \
          $(ROOT)/src/hashkey.c \
          $(ROOT)/src/str.c \
          $(ROOT)/src/option.c \
          $(ROOT)/src/async.c \
          $(ROOT)/src/subscribe.c \
          $(ROOT)/src/block.c \
          $(ROOT)/src/impl/st-node/mem.c \
          
# Required include directories
LIBINC += $(ROOT)/include \
          $(ROOT)/impl