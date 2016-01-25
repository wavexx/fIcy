## Makefile for fIcy
# Copyright(c) 2004-2016 by wave++ (Yuri D'Elia) <wavexx@thregr.org>

# Flags
CWARN += -Wall -Wextra -Wpedantic -Wno-unused-parameter
CXXFLAGS += -std=c++03 $(CWARN)
CPPFLAGS += -MD -D_FILE_OFFSET_BITS=64

# Paths
DESTDIR :=
PREFIX := /usr/local

# Objects/targets
TARGETS := fIcy fResync fPls
fIcy_OBJECTS := msg.o resolver.o socket.o http.o tmparse.o urlencode.o \
	base64.o urlparse.o hdrparse.o sanitize.o htfollow.o authparse.o \
	match.o icy.o rewrite.o fIcy.o
fResync_OBJECTS := msg.o mpeg.o copy.o fResync.o
fPls_OBJECTS := msg.o resolver.o socket.o http.o tmparse.o urlencode.o \
	base64.o urlparse.o hdrparse.o sanitize.o htfollow.o authparse.o \
	plsparse.o fPls.o


# Rules
.SUFFIXES:
.SECONDEXPANSION:
.PHONY: all clean install

all_OBJECTS := $(foreach T,$(TARGETS),$($(T)_OBJECTS))
all_DEPS := $(all_OBJECTS:.o=.d)
all: $(TARGETS)

%.o: %.cc
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

$(TARGETS): %: $$($$@_OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $($@_OBJECTS) $(LDFLAGS) $($@_LDADD)

clean:
	rm -f $(all_OBJECTS) $(all_DEPS) $(TARGETS)

install: $(TARGETS)
	install -p -t $(DESTDIR)$(PREFIX)/bin/ $(TARGETS)


# Dependencies
sinclude $(all_DEPS)
