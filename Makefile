# Makefile for fIcy (for pmake or gmake)
# Copyright(c) 2004 by wave++ (Yuri D'Elia) <wavexx@users.sf.net>

# configuration
TARGETS := fIcy
FICY_OBJECTS := fIcy.o resolver.o socket.o http.o \
	urlencode.o hdrparse.o icy.o sanitize.o

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


# stubs
.PHONY: all clean distclean
clean:
	rm -rf $(TARGETS) $(FICY_OBJECTS) ii_files core

distclean: clean
	rm -rf *~

sinclude $(DEPS)
