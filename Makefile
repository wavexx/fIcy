# Makefile for fIcy (for pmake or gmake)
# Copyright(c) 2004-2005 by wave++ (Yuri D'Elia) <wavexx@users.sf.net>

# configuration
TARGETS := fIcy fResync fPls
FICY_OBJECTS := msg.o resolver.o socket.o http.o tmparse.o urlencode.o \
	base64.o urlparse.o hdrparse.o sanitize.o htfollow.o authparse.o \
	match.o icy.o rewrite.o fIcy.o 
FRESYNC_OBJECTS := msg.o mpeg.o copy.o fResync.o
FPLS_OBJECTS := msg.o resolver.o socket.o http.o tmparse.o urlencode.o \
	base64.o urlparse.o hdrparse.o sanitize.o htfollow.o authparse.o \
	plsparse.o fPls.o

# parameters
DEPS := Makedepend
#if $(CXX) != "g++"
# This will be overriden when using GNU make, so let's assume MIPSPro.
# .ORDER is needed to avoid linking in parallel with -prelink
.ORDER: $(TARGETS)
DGEN := -MDupdate $(DEPS)
#else
DGEN := -MD
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

fPls: $(FPLS_OBJECTS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $(FPLS_OBJECTS)


# stubs
.PHONY: all clean distclean
clean:
	rm -rf $(TARGETS) $(FICY_OBJECTS) $(FRESYNC_OBJECTS) \
		 $(FPLS_OBJECTS) ii_files core $(DEPS) *.d

distclean: clean
	rm -rf *~

sinclude $(DEPS)
sinclude *.d
