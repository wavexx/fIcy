# Makefile for fIcy (for pmake or gmake)
# Copyright(c) 2004 by wave++ (Yuri D'Elia) <wavexx@users.sf.net>

# configuration
TARGETS := fIcy fResync
FICY_OBJECTS := fIcy.o resolver.o socket.o http.o msg.o \
	urlencode.o urlparse.o hdrparse.o icy.o sanitize.o
FRESYNC_OBJECTS := fResync.o msg.o mpeg.o copy.o

# parameters
DEPS := Makedepend
#if $(CXX) != "g++"
DGEN := -MDupdate $(DEPS)	# SGI MIPSPro
#else
DGEN := -MD -MF $(DEPS)		# GNU cc
#endif
CPPFLAGS += $(DGEN)


# suffixes, rules
.SUFFIXES: .cc .o
.cc.o:
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<


# targets
all: $(TARGETS)

fIcy: $(FICY_OBJECTS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $(FICY_OBJECTS)

fResync: $(FRESYNC_OBJECTS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $(FRESYNC_OBJECTS)


# stubs
.PHONY: all clean distclean
clean:
	rm -rf $(TARGETS) $(FICY_OBJECTS) $(FRESYNC_OBJECTS) \
		 ii_files core $(DEPS)

distclean: clean
	rm -rf *~

sinclude $(DEPS)
