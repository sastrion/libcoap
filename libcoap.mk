get-cwd = $(patsubst %/,%,$(dir $(abspath $(lastword $(MAKEFILE_LIST)))))
ROOT := $(call get-cwd)

# List of all the board related files.
EXTSRC += $(ROOT)/pdu.c \
         $(ROOT)/coap_net.c \
         $(ROOT)/debug.c \
         $(ROOT)/encode.c \
         $(ROOT)/uri.c \
         $(ROOT)/coap_list.c \
         $(ROOT)/resource.c \
         $(ROOT)/hashkey.c \
         $(ROOT)/str.c \
         $(ROOT)/option.c \
         $(ROOT)/async.c \
         $(ROOT)/subscribe.c \
         $(ROOT)/block.c \
         $(ROOT)/impl/st-node/mem.c

# Required include directories
EXTINC += $(ROOT) $(ROOT)/impl