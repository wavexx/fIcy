# configuration
TARGETS = fIcy
FICY_OBJECTS = fIcy.o resolver.o socket.o http.o \
	urlencode.o hdrparse.o icy.o sanitize.o

COMPILEXX = $(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<
LINKXX = $(CXX) $(LDFLAGS) -o $@

# suffixes, rules
.SUFFIXES: .cc .o
.cc.o:
	$(COMPILEXX)

# targets
all: $(TARGETS)

# executables
fIcy: $(FICY_OBJECTS)
	$(LINKXX) $(FICY_OBJECTS)

# fake rules
.PHONY: all clean distclean
clean:
	rm -rf $(TARGETS) $(FICY_OBJECTS) ii_files core

distclean: clean
	rm -rf *~

